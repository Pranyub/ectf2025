import claripy

from .MemoryRegion import MemoryRegion

# https://www.analog.com/media/en/technical-documentation/user-guides/max78000-user-guide.pdf
class MAX78000_UART_MemoryRegion(MemoryRegion):
    def __init__(self, start=0, size=0, mmio=False, name='???', use_concrete=True):
        super().__init__(start, size, mmio, name, use_concrete)
        self.register_offsets = {
            0x00: ('CTRL', self.__CR_read, self.__CR_write),
            0x04: ('STATUS', self.__STATUS_read, self.__STATUS_write),
            0x08: ('INT_EN', self.__INT_EN_read, self.__INT_EN_write),
            0x0C: ('INT_FL', self.__INT_FL_read, self.__INT_FL_write),
            0x10: ('CLKDIV', self.__CLKDIV_read, self.__CLKDIV_write),
            0x14: ('OSR', self.__OSR_read, self.__OSR_write),
            0x18: ('TXPEEK', self.__TXPEEK_read, self.__TXPEEK_write),
            0x1C: ('PNR', self.__PNR_read, self.__PNR_write),
            0x20: ('FIFO', self.__FIFO_read, self.__FIFO_write),
            0x30: ('DMA', self.__DMA_read, self.__DMA_write),
            0x34: ('WKEN', self.__WKEN_read, self.__WKEN_write),
            0x38: ('WKFL', self.__WKFL_read, self.__WKFL_write),
        }
    
    def __CR_read(self, state, addr, length):
        pass

    def __CR_write(self, state, addr, data):
        pass

    def __STATUS_read(self, state, addr, length):
        pass

    def __STATUS_write(self, state, addr, data):
        pass

    def __INT_EN_read(self, state, addr, length):
        pass

    def __INT_EN_write(self, state, addr, data):
        pass

    def __INT_FL_read(self, state, addr, length):
        pass

    def __INT_FL_write(self, state, addr, data):
        pass
    
    def __CLKDIV_read(self, state, addr, length):
        pass

    def __CLKDIV_write(self, state, addr, data):
        pass

    def __OSR_read(self, state, addr, length):
        pass

    def __OSR_write(self, state, addr, data):
        pass

    def __TXPEEK_read(self, state, addr, length):
        pass

    def __TXPEEK_write(self, state, addr, data):
        pass

    def __PNR_read(self, state, addr, length):
        pass

    def __PNR_write(self, state, addr, data):
        pass

    def __FIFO_read(self, state, addr, length):
        pass

    def __FIFO_write(self, state, addr, data):
        pass
    
    def __DMA_read(self, state, addr, length):
        pass
    
    def __DMA_write(self, state, addr, data):
        pass

    def __WKEN_read(self, state, addr, length):
        pass

    def __WKEN_write(self, state, addr, data):
        pass
    
    def __WKFL_read(self, state, addr, length):
        pass

    def __WKFL_write(self, state, addr, data):
        pass
