import angr
import archinfo
from avatar2 import Avatar, TargetStates, archs
from angr_targets import AvatarGDBConcreteTarget
import logging
import sys
import serial
import struct
from threading import Lock

from peripherals.MemoryRegion import *
from peripherals.PL011_UART_MemoryRegion import *
from peripherals.Pico_Timer import *
from peripherals.SimStateMemoryRegion import *
from peripherals.MAX78000Target import *

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


class HardwareTarget:

    CMD_MAGIC = b'\xcc'
    RESP_MAGIC = b'\xdd'
    CMD_READ_MEM = b'\x01'
    CMD_WRITE_MEM = b'\x02'
    CMD_SIZE = 0x0a

    def __init__(self, ip='victoryrd', port=3330, com='/dev/ttyPICO0', baud=115200, config='serial'):
        self.ip = ip
        self.port = port
        # self.ser = serial.Serial(com, baud)
        self.config = config
        self.lock = Lock() # angr is multithreaded, so we need to make sure interactions with the physical board are atomic

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

        regions = [self.sram, self.rom]

        regions += [
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
            PL011_UART_MemoryRegion(0x40070000, 0x8000, mmio=True, name="UART0_BASE", use_concrete=False),
            PL011_UART_MemoryRegion(0x40078000, 0x8000, mmio=True, name="UART1_BASE", use_concrete=False),
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
        
        # simstateplugins
        self.entry.register_plugin('memory_regions_plugin', SimStateMemoryRegion(regions))

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
    def get_region(self, state, addr):
        for region in state.memory_regions_plugin.regions:
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
        region = self.get_region(state, addr)
        if not region:
            print(f'mmio read: {hex(addr)} unmapped region')
            return
        elif not region.mmio:
            return

        print(f'mmio read: {hex(addr)} {region.name}')
        region.read(state, addr)

        if self.concrete_target and region.use_concrete:
            read_bytes = self.concrete_target.read_memory(addr, 4)
            state.memory.store(addr, read_bytes)
            print(f'   read using concrete target: {hex(addr)} -> {read_bytes.hex()}')

        debug_stack_trace(state, depth=1)

    def write_hook(self, state):
        write_addr = state.inspect.mem_write_address
        write_data = state.solver.eval(state.inspect.mem_write_expr)
        addr = state.solver.eval(write_addr)
        region = self.get_region(state, addr)
        if not region:
            print(f'mmio write: {hex(addr)} unmapped region')
            return
        elif not region.mmio:
            return

        print(f'mmio write: {hex(write_data)} -> {hex(addr)} {region.name}')
        region.write(state, addr, write_data)

        if self.concrete_target and region.use_concrete:
            self.concrete_target.write_memory(addr, write_data)
            print(f'   write using concrete target: {hex(write_data)} -> {hex(addr)}')
        debug_stack_trace(state, depth=1)
        

class MAX78000Target:
    def __init__(self, binary, concrete_target=None, cached_cfg=None):
        # sram memory regions
        self.sram = MemoryRegion(0x20000000, 0x1FFFFFFF, mmio=False, name='SRAM', use_concrete=False)
        self.rom = MemoryRegion(0x00000000, 0x1FFFFFFF, mmio=False, name='ROM', use_concrete=False)

        regions = [self.sram, self.rom]

        # add other memory regions
        regions += [
            MemoryRegion(0x400BE000, 0x03FF, mmio=True, name='SPIO'),
            MemoryRegion(0x400C0400, 0x0000, mmio=True, name='CNN_FIFO'),
            MemoryRegion(0x40000000, 0x03FF, mmio=True, name='GCR'),
            MemoryRegion(0x40000400, 0x03FF, mmio=True, name='SIR'),
            MemoryRegion(0x40000800, 0x03FF, mmio=True, name='FCR'),
            MemoryRegion(0x40003000, 0x03FF, mmio=True, name='WDT0'),
            MemoryRegion(0x40003C00, 0x003F, mmio=True, name='DVS'),
            MemoryRegion(0x40004400, 0x03FF, mmio=True, name='SIMO'),
            MemoryRegion(0x40005400, 0x03FF, mmio=True, name='TRIMSIR'),
            MemoryRegion(0x40005800, 0x03FF, mmio=True, name='GCFR'),
            MemoryRegion(0x40006000, 0x03FF, mmio=True, name='RTC'),
            MemoryRegion(0x40006400, 0x03FF, mmio=True, name='WUT'),
            MemoryRegion(0x40006800, 0x03FF, mmio=True, name='PWRSEQ'),
            MemoryRegion(0x40006C00, 0x03FF, mmio=True, name='MCR'),
            MemoryRegion(0x40007400, 0x03FF, mmio=True, name='AES'),
            MemoryRegion(0x40007800, 0x03FF, mmio=True, name='AESKEY'),
            MemoryRegion(0x40008000, 0x0FFF, mmio=True, name='GPIO0'),
            MemoryRegion(0x40009000, 0x0FFF, mmio=True, name='GPIO1'),
            MemoryRegion(0x4000E000, 0x0FFF, mmio=True, name='PCIF'),
            MemoryRegion(0x4000F000, 0x0FFF, mmio=True, name='CRC'),
            MemoryRegion(0x40010000, 0x0FFF, mmio=True, name='TMR0'),
            MemoryRegion(0x40011000, 0x0FFF, mmio=True, name='TMR1'),
            MemoryRegion(0x40012000, 0x0FFF, mmio=True, name='TMR2'),
            MemoryRegion(0x40013000, 0x0FFF, mmio=True, name='TMR3'),
            MemoryRegion(0x4001D000, 0x0FFF, mmio=True, name='I2C0'),
            MemoryRegion(0x4001E000, 0x0FFF, mmio=True, name='I2C1'),
            MemoryRegion(0x4001F000, 0x0FFF, mmio=True, name='I2C2'),
            MemoryRegion(0x40028000, 0x0FFF, mmio=True, name='DMA'),
            MemoryRegion(0x40029000, 0x03FF, mmio=True, name='FLC0'),
            MemoryRegion(0x4002A000, 0x07FF, mmio=True, name='ICC0'),
            MemoryRegion(0x4002A800, 0x07FF, mmio=True, name='ICC1'),
            MemoryRegion(0x40034000, 0x0FFF, mmio=True, name='ADC'),
            MemoryRegion(0x4003C000, 0x009F, mmio=True, name='PT'),
            MemoryRegion(0x4003D000, 0x0FFF, mmio=True, name='OWM0'),
            MemoryRegion(0x4003E000, 0x0FFF, mmio=True, name='SEMA'),
            MAX78000_UART_MemoryRegion(0x40042000, 0x0FFF, mmio=True, name='UART0', use_concrete=False),
            MAX78000_UART_MemoryRegion(0x40043000, 0x0FFF, mmio=True, name='UART1', use_concrete=False),
            MAX78000_UART_MemoryRegion(0x40044000, 0x0FFF, mmio=True, name='UART2', use_concrete=False),
            MemoryRegion(0x40046000, 0x1FFF, mmio=True, name='SPI1'),
            MemoryRegion(0x4004D000, 0x0FFF, mmio=True, name='TRNG'),
            MemoryRegion(0x40060000, 0x0FFF, mmio=True, name='I2S'),
            MemoryRegion(0x40080000, 0x03FF, mmio=True, name='LPGCR'),
            MemoryRegion(0x40080400, 0x01FF, mmio=True, name='GPIO2'),
            MemoryRegion(0x40080800, 0x03FF, mmio=True, name='WDT1'),
            MemoryRegion(0x40080C00, 0x03FF, mmio=True, name='TMR4'),
            MemoryRegion(0x40081000, 0x03FF, mmio=True, name='TMR5'),
            MemoryRegion(0x40081400, 0x03FF, mmio=True, name='UART3'),  # LPUART
            MemoryRegion(0x40088000, 0x03FF, mmio=True, name='LPCMP'),
            MemoryRegion(0x50000000, 0x000FFFFF, mmio=True, name='CNN'),
            MemoryRegion(0x50100000, 0x003FFFFF, mmio=True, name='CNNx16_0'),
            MemoryRegion(0x50500000, 0x003FFFFF, mmio=True, name='CNNx16_1'),
            MemoryRegion(0x50900000, 0x003FFFFF, mmio=True, name='CNNx16_2'),
            MemoryRegion(0x50D00000, 0x003FFFFF, mmio=True, name='CNNx16_3')
        ]

        if concrete_target != None:
            self.concrete_target = concrete_target
        else:
            self.concrete_target = None

        self.proj = angr.Project(
            binary,
            load_options={'auto_load_libs': False},
            arch='ARMCortexM',
        )

        # "program memory area must also contain the default system vector table and the initial settings for all system
        # exception handlers and interrupt handlers for the CM4 core. The reset vector for the device is 0x0000 0000 and contains
        # the device ROM code that transfers execution to user code at address 0x1000 0000."

        # debug startup issues here, like on the Pico

        entry_options={angr.options.SYMBOL_FILL_UNCONSTRAINED_MEMORY, angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS}
        self.entry = self.proj.factory.entry_state(add_options=entry_options)
        
        # simstateplugins
        self.entry.register_plugin('memory_regions_plugin', SimStateMemoryRegion(regions))

        self.entry.inspect.b('mem_read', when=angr.BP_BEFORE, action=self.read_hook)
        self.entry.inspect.b('mem_write', when=angr.BP_BEFORE, action=self.write_hook)

        self.simgr = self.proj.factory.simulation_manager(self.entry)
        self.cfg = self.proj.analyses.CFGFast()

        # Parse initial stack pointer and reset vector
        text_section = self.proj.loader.main_object.sections_map.get('.text')
        self.vector_table = self.proj.loader.memory.load(text_section.min_addr, 8)
        self.initial_sp = int.from_bytes(self.vector_table[0:4], byteorder='little')
        self.reset_vector = int.from_bytes(self.vector_table[4:8], byteorder='little')

    def get_region(self, state, addr):
        for region in state.memory_regions_plugin.regions:
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
        region = self.get_region(state, addr)
        if not region:
            print(f'mmio read: {hex(addr)} unmapped region')
            return
        elif not region.mmio:
            return

        print(f'mmio read: {hex(addr)} {region.name}')
        region.read(state, addr)

        if self.concrete_target and region.use_concrete:
            read_bytes = self.concrete_target.read_memory(addr, 4)
            state.memory.store(addr, read_bytes)
            print(f'   read using concrete target: {hex(addr)} -> {read_bytes.hex()}')

        debug_stack_trace(state, depth=1)
        
    def write_hook(self, state):
        write_addr = state.inspect.mem_write_address
        write_data = state.solver.eval(state.inspect.mem_write_expr)
        addr = state.solver.eval(write_addr)
        region = self.get_region(state, addr)
        if not region:
            print(f'mmio write: {hex(addr)} unmapped region')
            return
        elif not region.mmio:
            return

        print(f'mmio write: {hex(write_data)} -> {hex(addr)} {region.name}')
        region.write(state, addr, write_data)

        if self.concrete_target and region.use_concrete:
            self.concrete_target.write_memory(addr, write_data)
            print(f'   write using concrete target: {hex(write_data)} -> {hex(addr)}')
        debug_stack_trace(state, depth=1)