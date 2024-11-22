import claripy

from .MemoryRegion import MemoryRegion

class PL011_UART_MemoryRegion(MemoryRegion):
    def __init__(self, start=0, size=0, mmio=False, name='???', use_concrete=True):
        super().__init__(start, size, mmio, name, use_concrete)
        self.register_offsets = {
            0x00: ('DR', self.__DR_read, self.__DR_write),  # data register
            0x18: ('FR', self.__FR_read, self.__FR_write),  # flag register
            0x24: ('IBRD', self.__IBRD_read, self.__IBRD_write),  # baud rate
            0x28: ('FBRD', self.__FBRD_read, self.__FBRD_write),  # baud rate
            0x2c: ('LCR_H', self.__LCR_H_read, self.__LCR_H_write),  # data frame format
            0x30: ('CR', self.__CR_read, self.__CR_write),  # control register
            0x38: ('IMSC', self.__IMSC_read, self.__IMSC_write),  # enable/disable interrupts
            0x48: ('DMACR', self.__DMARC_read, self.__DMARC_write)  # enable/disable DMA
        }
        self.read_write_hook_active = False
        self.sent = []
        self.received = []
        

    def __update_FR(self, state, data=None):
        # bit 0: CTS: clear to send, complement of nUARTCTS, not sure what is should be
        # bit 1: DSR: data set ready, complement of nUARTDSR, not sure what it should be
        # bit 2: DCD: data carrier detect, complement of nUARTDCD, not sure what it should be
        # bit 3: BUSY: UART busy, should always be 0 for emulation
        # bit 4: RXFE: receive FIFO empty, should always be 1 for emulation
        # bit 5: TXFE: transmit FIFO full, should always be 0 for emulation
        # bit 6: RXFF: receive FIFO full, should always be 0 for emulation
        # bit 7: TXFE: transmit FIFO empty, should always be 1 for emulation
        # bit 8: RI: ring indicator, complement of nUARTRI, not sure what it should be
        # bits 9-15: leave as 0
        if self.read_write_hook_active:  # prevents infinite recursion
            return
        self.read_write_hook_active = True
        addr = self.base + 0x18
        output = 0b0000100110000000
        if data:
            output = data
        out_mask = 0b1111111110000000  # used to prevent remaining bits from being set
        output = output & out_mask
        print(f"Flag register: {output:b}")
        state.memory.store(addr, claripy.BVV(output, 16), endness='big')
        self.read_write_hook_active = False
    
    def __DR_read(self, state, addr):
        print(f"\"Reading\" 0x{data:x} over UART")
        self.received.append(data)
        self.__update_FR(state)
    
    def __DR_write(self, state, addr, data):
        print(f"\"Sending\" 0x{data:x} over UART")
        self.sent.append(data)
        self.__update_FR(state)
    
    def __FR_read(self, state, addr):
        pass

    def __FR_write(self, state, addr, data):
        # this probably should never get called
        self.__update_FR(state, data)
        print("!!!!!!!!!!!!!!!!!!!!!!!! WRITE TO UART FLAG REGISTER !!!!!!!!!!!!!!!!!!!!!!!!")
        pass

    def __IBRD_read(self, state, addr):
        # reading the integer part of the clock speed
        pass
    
    def __IBRD_write(self, state, addr, data):
        # setting the integer part of the clock speed
        pass

    def __FBRD_read(self, state, addr):
        # reading the fractional part of the clock speed
        pass

    def __FBRD_write(self, state, addr, data):
        # writing the fractional part of the clock speed
        pass
    
    def __LCR_H_read(self, state, addr):
        pass

    def __LCR_H_write(self, state, addr, data):
        pass

    def __CR_read(self, state, addr):
        # bit 9 indicates that the receiver is on.
        pass
    
    def __CR_write(self, state, addr, data):
        pass

    def __IMSC_read(self, state, addr):
        pass

    def __IMSC_write(self, state, addr, data):
        pass

    def __DMARC_read(self, state, addr):
        pass

    def __DMARC_write(self, state, addr, data):
        pass

    def read(self, state, addr, size=4):
        assert size == 4
        offset = addr - self.base
        if offset in self.register_offsets:
            print(f"Read of UART{self.register_offsets[offset][0]} register")
            self.register_offsets[offset][1](state, addr)
        else:
            print(f"Read of unknown UART register at address 0x{addr:x} with an offset of 0x{offset:x}")
        return

    def write(self, state, addr, data):
        offset = addr - self.base
        if offset in self.register_offsets:
            print(f"Write to UART{self.register_offsets[offset][0]} register: 0x{data:x}")
            self.register_offsets[offset][2](state, addr, data)
        else:
            print(f"Write to unknown UAR register at address 0x{addr:x}: 0x{data:x}")
        return
