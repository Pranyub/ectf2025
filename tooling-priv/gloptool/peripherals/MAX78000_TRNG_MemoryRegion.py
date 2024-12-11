import claripy
import os

from .MemoryRegion import MemoryRegion

class MAX78000_TRNG_MemoryRegion(MemoryRegion):
    def __init__(self, start=0, size=0, mmio=False, name='???', use_concrete=True):
        super().__init__(start, size, mmio, name, use_concrete)
        self.register_offsets = {
            0x00: ('CTRL', self.__CTRL_read, self.__CTRL_write, 0),
            0x04: ('STATUS', self.__STATUS_read, self.__STATUS_write, 0),
            0x08: ('DATA', self.__DATA_read, self.__DATA_write, 0)
        }
        self.read_write_hook_active = False
    
    def __read_state(self, state, addr, length):
        if self.read_write_hook_active:  # prevents infinite recursion
            return None
        self.read_write_hook_active = True
        output = state.memory.load(addr, length)
        self.read_write_hook_active = False
        return state.solver.eval(output)

    def __write_state(self, state, addr, data):
        if self.read_write_hook_active:  # prevents infinite recursion
            return
        self.read_write_hook_active = True
        value = claripy.BVV(data, 32)  # pad to 4 bytes, the length of each register
        state.memory.store(addr, value, endness=state.arch.memory_endness)
        self.read_write_hook_active = False
    
    def __CTRL_read(self, state, addr):
        # 31-16 (- RO): reserved
        # 15 (keywipe R/W): wipe key, write 1 to wipe the TRNG key
        # 14-4 (- RO): reserved
        # 3 (keygen R/W): generate key, write 1 to generate a key
        # 2 (- RO): reserved
        # 1 (rnd_ie R/W): random number interrupt enable, 0 disabled, 1 enabled.  set to 0
        # 0 (- RO): reserved
        self.__write_state(state, addr, self.__register_offsets[addr][3])

    def __CTRL_write(self, state, addr, data):
        # we can ignore because we'll generate when the data field is read
        pass

    def __STATUS_read(self, state, addr):
        # 31-1 (- RO): reserved
        # 0 (rdy R): key ready to be read, 0 not ready, 1 ready.  set to 1
        self.__write_state(state, addr, 1)
        pass

    def __STATUS_write(self, state, addr, data):
        # should never happen
        pass

    def __DATA_read(self, state, addr):
        # 31-0 (data RO): the 32-bit random number
        self.__write_state(state, addr, os.urandom(4))
        pass

    def __DATA_write(self, state, addr, data):
        # should never happen
        pass

    def read(self, state, addr, size=4):
        if self.read_write_hook_active:
            return
        assert size == 4
        offset = addr - self.base
        if offset in self.register_offsets:
            # print(f"Read of {self.name}{self.register_offsets[offset][0]} register")
            self.register_offsets[offset][1](state, addr)
        else:
            print(f"WARNING: read of unknown {self.name} register at address 0x{addr:x} with an offset of 0x{offset:x} !")
    
    def write(self, state, addr, data):
        if self.read_write_hook_active:
            return
        offset = addr - self.base
        if offset in self.register_offsets:
            # print(f"Write to {self.name}{self.register_offsets[offset][0]} register: 0x{data:x}")
            self.register_offsets[offset][2](state, addr, data)
        else:
            print(f"WARNING: write to unknown {self.name} register at address 0x{addr:x}: 0x{data:x} !")