from peripherals import MemoryRegion

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