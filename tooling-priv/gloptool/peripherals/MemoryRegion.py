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
