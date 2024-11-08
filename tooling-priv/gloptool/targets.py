import angr
import archinfo
from avatar2 import Avatar, TargetStates, archs
from angr_targets import AvatarGDBConcreteTarget
import logging
import sys
import serial

class HardwareTarget:
    def __init__(self, ip='victoryrd', port=3330, com='/dev/ttyPICO0', baud=115200):
        self.ip = ip
        self.port = port

        self.avatar = AvatarGDBConcreteTarget(
            architecture=archs.arm.ARM,
            gdbserver_ip=self.ip,
            gdbserver_port=self.port
        )

        self.ser = serial.Serial(com, baud)

class PicoTarget:
    def __init__(self, binary, concrete_target=None, cached_cfg=None):

        # sram memory regions
        self.sram = [range(0x20000000, 0x20080000)]
        self.rom = [range(0x00000000, 0x1FFFFFFF)]

        # Set up concrete target if given
        if concrete_target != None:
            self.concrete_target = concrete_target
            self.gdb_target = concrete_target.gdb_target
        else:
            self.avatar = None
            self.gdb_target = None

        self.proj = angr.Project(
            binary,
            concrete_target=self.gdb_target,
            load_options={'auto_load_libs': False},
            arch='ARMCortexM',
        )

        # Copy some data from .code to 0x00000000
        # ??? wtf is at 0x00000000
        data = self.proj.loader.memory.load(0x10000000, 0x1000)
        self.proj.loader.memory.add_backer(0x0000000, data)

        #for entry in self.rom:
        #    self.proj.loader.memory.add_backer(min(entry),  max(entry))

        entry_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY, angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS}
        self.entry = self.proj.factory.entry_state(add_options=entry_options)

        #self.entry.inspect.b('mem_read', when=angr.BP_AFTER, action=self.read_hook)

        self.simgr = self.proj.factory.simulation_manager(self.entry)
        self.cfg = self.proj.analyses.CFGFast()

        # Parse initial stack pointer and reset vector
        text_section = self.proj.loader.main_object.sections_map.get('.text')
        self.vector_table = self.proj.loader.memory.load(text_section.min_addr, 8)
        self.initial_sp = int.from_bytes(self.vector_table[0:4], byteorder='little')
        self.reset_vector = int.from_bytes(self.vector_table[4:8], byteorder='little')

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
            page_data = state.memory.load(page_start, page_size)
            self.gdb_target.write_memory(page_start, page_data, size=page_size)

    def run(self):
        self.simgr.run(n=100)

    def read_hook(self, state):
        read_addr = state.inspect.mem_read_address
        logging.info(f'read_hook')
        print(f'here: {read_addr} {state.regs.pc}')
