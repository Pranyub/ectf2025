import angr
import archinfo
from avatar2 import Avatar, TargetStates, archs
from angr_targets import AvatarGDBConcreteTarget
import logging
import sys
import serial
import struct
from threading import Lock

def debug_stack_trace(state, depth=3):
    print("Stack trace:")
    for frame in state.callstack:
        func = state.project.kb.functions.get(frame.func_addr)
        if func:
            function_name = func.name
        else:
            function_name = '???'
        print(f"Function: {function_name}, Address: 0x{frame.func_addr:x}, Return Address: 0x{frame.ret_addr:x}")
        depth -= 1
        if depth == 0:
            return

# Abstract class representing mmio region
class MemoryRegion:
    def __init__(self, start=0, size=0, mmio=False, name='???', use_concrete=True):
        self.bounds = range(start, start + size)
        self.base = start
        self.size = size
        self.end = start + size
        self.mmio = mmio
        self.name = name
        self.use_concrete = use_concrete

    def has(self, addr):
        return addr in self.bounds

    def read(self, state, addr, size=4):
        return
    
    def write(self, state, addr, data):
        return

    # This is called if a read/write is called to a not implemented addr
    def not_impl(self):
        raise Exception('not yet implemented')

class Pico_Timer(MemoryRegion):
    def __init__(self, start=0, size=0, mmio=False, name='???'):
        super().__init__(start, size, mmio, name)
        self.vars = {
            0x00: 0,
            0x04: 0,
        }

    def read(self, state, addr, size=4):
        assert size == 4
        offset = self.start - addr

    def write(self, state, addr, data):
        pass

class HardwareTarget:

    CMD_MAGIC = b'\xcc'
    RESP_MAGIC = b'\xdd'
    CMD_READ_MEM = b'\x01'
    CMD_WRITE_MEM = b'\x02'
    CMD_SIZE = 0x0a

    def __init__(self, ip='victoryrd', port=3330, com='/dev/ttyPICO0', baud=115200, config='serial'):
        self.ip = ip
        self.port = port
        self.ser = serial.Serial(com, baud)
        self.config = config
        self.lock = Lock()

        if config == 'gdb':
            self.avatar = AvatarGDBConcreteTarget(
                architecture=archs.arm.ARM,
                gdbserver_ip=self.ip,
                gdbserver_port=self.port
            )
            self.target = self.avatar.target

            if self.avatar.is_running():
                self.avatar.stop()

        

    def read_memory(self, address, nbytes, **kwargs):
        self.lock.acquire()
        if self.config == 'gdb':
            ret = self.avatar.target.read_memory(address, 1, int(nbytes), raw=True)
        elif self.config == 'serial':
            assert nbytes == 4
            cmd = HardwareTarget.CMD_MAGIC + HardwareTarget.CMD_READ_MEM + struct.pack('>I', address)
            self.ser.write(cmd.ljust(HardwareTarget.CMD_SIZE))
            resp = self.ser.read(HardwareTarget.CMD_SIZE)
            #print(resp.hex())
            #assert resp[0] == HardwareTarget.RESP_MAGIC and resp[1] == HardwareTarget.CMD_READ_MEM
            ret = resp[2:6]
        
        else:    
            raise Exception(f'config {self.config} is not yet implemented')
        self.lock.release()
        return ret

    def write_memory(self, address, value, **kwargs):
        self.lock.acquire()
        if self.config == 'gdb':
            self.target.write_memory(address, 1, value, raw=True)
        elif self.config == 'serial':
            assert value in range(0, 1 << 32)
            cmd = HardwareTarget.CMD_MAGIC + HardwareTarget.CMD_WRITE_MEM + struct.pack('>I', address) + struct.pack('>I', value)
            self.ser.write(cmd.ljust(HardwareTarget.CMD_SIZE))
            resp = self.ser.read(HardwareTarget.CMD_SIZE)
            #assert resp[0] == HardwareTarget.RESP_MAGIC and resp[1] == HardwareTarget.CMD_WRITE_MEM

        else:    
            raise Exception(f'config {self.config} is not yet implemented')
        self.lock.release()
class PicoTarget:

    def __init__(self, binary, concrete_target=None, cached_cfg=None):

        # sram memory regions
        self.sram = MemoryRegion(0x20000000, 0x82000, mmio=False, name='SRAM', use_concrete=False)
        self.rom = MemoryRegion(0x00000000, 0x1FFFFFFF, mmio=False, name='ROM', use_concrete=False)

        self.regions = [self.sram, self.rom]

        self.regions += [
            # AHB regions
            MemoryRegion(0x40000000, 0x8000, mmio=True, name="SYSINFO_BASE"),
            MemoryRegion(0x40008000, 0x8000, mmio=True, name="SYSCFG_BASE"),
            MemoryRegion(0x40010000, 0x8000, mmio=True, name="CLOCKS_BASE"),
            MemoryRegion(0x40018000, 0x8000, mmio=True, name="PSM_BASE"),
            MemoryRegion(0x40020000, 0x8000, mmio=True, name="RESETS_BASE", use_concrete=False),
            MemoryRegion(0x40028000, 0x8000, mmio=True, name="IO_BANK0_BASE", use_concrete=False),
            MemoryRegion(0x40030000, 0x8000, mmio=True, name="IO_QSPI_BASE"),
            MemoryRegion(0x40038000, 0x8000, mmio=True, name="PADS_BANK0_BASE", use_concrete=False),
            MemoryRegion(0x40040000, 0x8000, mmio=True, name="PADS_QSPI_BASE"),
            MemoryRegion(0x40048000, 0x8000, mmio=True, name="XOSC_BASE"),
            MemoryRegion(0x40050000, 0x8000, mmio=True, name="PLL_SYS_BASE", use_concrete=False),
            MemoryRegion(0x40058000, 0x8000, mmio=True, name="PLL_USB_BASE", use_concrete=False),
            MemoryRegion(0x40060000, 0x8000, mmio=True, name="ACCESSCTRL_BASE", use_concrete=False),
            MemoryRegion(0x40068000, 0x8000, mmio=True, name="BUSCTRL_BASE", use_concrete=False),
            MemoryRegion(0x40070000, 0x8000, mmio=True, name="UART0_BASE", use_concrete=False),
            MemoryRegion(0x40078000, 0x8000, mmio=True, name="UART1_BASE", use_concrete=False),
            MemoryRegion(0x40080000, 0x8000, mmio=True, name="SPI0_BASE"),
            MemoryRegion(0x40088000, 0x8000, mmio=True, name="SPI1_BASE"),
            MemoryRegion(0x40090000, 0x8000, mmio=True, name="I2C0_BASE"),
            MemoryRegion(0x40098000, 0x8000, mmio=True, name="I2C1_BASE"),
            MemoryRegion(0x400A0000, 0x8000, mmio=True, name="ADC_BASE"),
            MemoryRegion(0x400A8000, 0x8000, mmio=True, name="PWM_BASE"),
            MemoryRegion(0x400B0000, 0x8000, mmio=True, name="TIMER0_BASE"),
            MemoryRegion(0x400B8000, 0x8000, mmio=True, name="TIMER1_BASE"),
            MemoryRegion(0x400C0000, 0x8000, mmio=True, name="HSTX_CTRL_BASE"),
            MemoryRegion(0x400C8000, 0x8000, mmio=True, name="XIP_CTRL_BASE", use_concrete=False),
            MemoryRegion(0x400D0000, 0x8000, mmio=True, name="XIP_QMI_BASE", use_concrete=False),
            MemoryRegion(0x400D8000, 0x8000, mmio=True, name="WATCHDOG_BASE", use_concrete=False),
            MemoryRegion(0x400E0000, 0x8000, mmio=True, name="BOOTRAM_BASE", use_concrete=False),
            MemoryRegion(0x400E8000, 0x8000, mmio=True, name="ROSC_BASE"),
            MemoryRegion(0x400F0000, 0x8000, mmio=True, name="TRNG_BASE"),
            MemoryRegion(0x400F8000, 0x8000, mmio=True, name="SHA256_BASE"),
            MemoryRegion(0x40100000, 0x8000, mmio=True, name="POWMAN_BASE"),
            MemoryRegion(0x40108000, 0x8000, mmio=True, name="TICKS_BASE"),
            MemoryRegion(0x40120000, 0x8000, mmio=True, name="OTP_BASE", use_concrete=False),
            MemoryRegion(0x40130000, 0x4000, mmio=True, name="OTP_DATA_BASE", use_concrete=False),
            MemoryRegion(0x40134000, 0x4000, mmio=True, name="OTP_DATA_RAW_BASE", use_concrete=False),
            MemoryRegion(0x40138000, 0x4000, mmio=True, name="OTP_DATA_GUARDED_BASE", use_concrete=False),
            MemoryRegion(0x4013C000, 0x4000, mmio=True, name="OTP_DATA_RAW_GUARDED_BASE", use_concrete=False),
            MemoryRegion(0x40140000, 0x2000, mmio=True, name="CORESIGHT_PERIPH_BASE", use_concrete=False),
            MemoryRegion(0x40140000, 0x2000, mmio=True, name="CORESIGHT_ROMTABLE_BASE", use_concrete=False),
            MemoryRegion(0x40142000, 0x2000, mmio=True, name="CORESIGHT_AHB_AP_CORE0_BASE", use_concrete=False),
            MemoryRegion(0x40144000, 0x2000, mmio=True, name="CORESIGHT_AHB_AP_CORE1_BASE", use_concrete=False),
            MemoryRegion(0x40146000, 0x1000, mmio=True, name="CORESIGHT_TIMESTAMP_GEN_BASE", use_concrete=False),
            MemoryRegion(0x40147000, 0x1000, mmio=True, name="CORESIGHT_ATB_FUNNEL_BASE", use_concrete=False),
            MemoryRegion(0x40148000, 0x1000, mmio=True, name="CORESIGHT_TPIU_BASE", use_concrete=False),
            MemoryRegion(0x40149000, 0x1000, mmio=True, name="CORESIGHT_CTI_BASE", use_concrete=False),
            MemoryRegion(0x4014A000, 0x1000, mmio=True, name="CORESIGHT_APB_AP_RISCV_BASE", use_concrete=False),
            MemoryRegion(0x40158000, 0x8000, mmio=True, name="GLITCH_DETECTOR_BASE", use_concrete=False),
            MemoryRegion(0x40160000, 0x8000, mmio=True, name="TBMAN_BASE"),

            # SIO Regions
            MemoryRegion(0xd0000000, 0x20000, mmio=True, name="SIO_BASE"),
            MemoryRegion(0xd0020000, 0x20000, mmio=True, name="SIO_NONSEC_BASE"),
        ]

        # Set up concrete target if given
        if concrete_target != None:
            self.concrete_target = concrete_target
        else:
            self.concrete_target = None

        self.proj = angr.Project(
            binary,
            load_options={'auto_load_libs': False},
            arch='ARMCortexM',
        )

        # Copy some data from .code to 0x00000000
        # ??? wtf is at 0x00000000
        data = self.proj.loader.memory.load(0x10000000, 0x10000000)
        self.proj.loader.memory.add_backer(0x0000000, data)
        #self.proj.loader.memory.add_backer(self.sram.base, b'\x00' * self.sram.size)

        #for entry in self.rom:
        #    self.proj.loader.memory.add_backer(min(entry),  max(entry))

        entry_options={angr.options.SYMBOL_FILL_UNCONSTRAINED_MEMORY, angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS}
        self.entry = self.proj.factory.entry_state(add_options=entry_options)

        self.entry.inspect.b('mem_read', when=angr.BP_BEFORE, action=self.read_hook)
        self.entry.inspect.b('mem_write', when=angr.BP_BEFORE, action=self.write_hook)

        self.simgr = self.proj.factory.simulation_manager(self.entry)
        self.cfg = self.proj.analyses.CFGFast()

        # Parse initial stack pointer and reset vector
        text_section = self.proj.loader.main_object.sections_map.get('.text')
        self.vector_table = self.proj.loader.memory.load(text_section.min_addr, 8)
        self.initial_sp = int.from_bytes(self.vector_table[0:4], byteorder='little')
        self.reset_vector = int.from_bytes(self.vector_table[4:8], byteorder='little')

    # Returns region of address or none if address is not mapped
    def get_region(self, addr):
        for region in self.regions:
            if region.has(addr):
                return region
        
        return None


    # Runs a given state on a concrete target
    # Does nothing if there is no avatar set up
    def run_concrete(self, state=None):
        if self.avatar == None:
            return

        if state == None:
            state = self.entry
        
        # Set regs on concrete target
        for reg in state.arch.register_list:
            reg_name = reg.name
            reg_value = state.registers.load(reg_name)
            self.gdb_target.write_register(reg_name, reg_value)

        # Set memory on concrete target
        for page_start in state.memory.changed_bytes:
            page_size = len(state.memory.changed_bytes[page_start])
            page_data = state. memory.load(page_start, page_size)
            self.gdb_target.write_memory(page_start, page_data, size=page_size)

    def run(self, n=1000):
        self.simgr.run(n=n)

    def read_hook(self, state):
        read_addr = state.inspect.mem_read_address
        addr = state.solver.eval(read_addr)
        region = self.get_region(addr)
        if not region:
            print(f'mmio read: {hex(addr)} unmapped region')
            return
        elif not region.mmio:
            return

        print(f'mmio read: {hex(addr)} {region.name}')

        if self.concrete_target and region.use_concrete:
            read_bytes = self.concrete_target.read_memory(addr, 4)
            state.memory.store(addr, read_bytes)
            print(f'   read using concrete target: {hex(addr)} -> {read_bytes.hex()}')

        debug_stack_trace(state, depth=1)

    def write_hook(self, state):
        write_addr = state.inspect.mem_write_address
        write_data = state.solver.eval(state.inspect.mem_write_expr)
        addr = state.solver.eval(write_addr)
        region = self.get_region(addr)
        if not region:
            print(f'mmio write: {hex(addr)} unmapped region')
            return
        elif not region.mmio:
            return

        print(f'mmio write: {hex(write_data)} -> {hex(addr)} {region.name}')

        if self.concrete_target and region.use_concrete:
            self.concrete_target.write_memory(addr, write_data)
            print(f'   write using concrete target: {hex(write_data)} -> {hex(addr)}')
        debug_stack_trace(state, depth=1)
        
