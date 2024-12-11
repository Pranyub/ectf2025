import claripy

from .MemoryRegion import MemoryRegion

class MAX78000_AES_MemoryRegion(MemoryRegion):
    def __init__(self, start=0, size=0, mmio=False, name='???', use_concrete=True):
        super().__init__(start, size, mmio, name, use_concrete)
        self.register_offsets = {
            0x00: ("CTRL", self.__CTRL_read, self.__CTRL_write),
            0x04: ("STATUS", self.__STATUS_read, self.__STATUS_write),
            0x08: ("INTFL", self.__INTFL_read, self.__INTFL_write),
            0x0C: ("INTEN", self.__INTEN_read, self.__INTEN_write),
            0x10: ("FIFO", self.__FIFO_read, self.__FIFO_write)
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

    def read(self, state, addr, size=4):
        if self.read_write_hook_active:
            return
        assert size == 4
        offset = addr - self.base
        if offset in self.register_offsets:
            print(f"Read of {self.name}{self.register_offsets[offset][0]} register")
            self.register_offsets[offset][1](state, addr)
        else:
            print(f"WARNING: read of unknown {self.name} register at address 0x{addr:x} with an offset of 0x{offset:x} !")
    
    def write(self, state, addr, data):
        if self.read_write_hook_active:
            return
        offset = addr - self.base
        if offset in self.register_offsets:
            print(f"Write to {self.name}{self.register_offsets[offset][0]} register: 0x{data:x}")
            self.register_offsets[offset][2](state, addr, data)
        else:
            print(f"WARNING: write to unknown {self.name} register at address 0x{addr:x}: 0x{data:x} !")

    def __CTRL_read(self, state, addr):
        pass

    def __CTRL_write(self, state, addr, data):
        pass

    def __STATUS_read(self, state, addr):
        pass

    def __STATUS_write(self, state, addr, data):
        pass

    def __INTFL_read(self, state, addr):
        pass

    def __INTFL_write(self, state, addr, data):
        pass

    def __INTEN_read(self, state, addr):
        pass

    def __INTEN_write(self, state, addr, data):
        pass

    def __FIFO_read(self, state, addr):
        pass

    def __FIFO_write(self, state, addr, data):
        pass