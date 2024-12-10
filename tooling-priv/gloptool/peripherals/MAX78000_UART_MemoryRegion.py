import claripy

from .MemoryRegion import MemoryRegion

# https://www.analog.com/media/en/technical-documentation/user-guides/max78000-user-guide.pdf
# memory access meanings:
# RO: reserved, reads reset the field to the default values, writes are ignored
# DNM: reserved, do not write, reads must write the same value afterwards
# R: read only, writes are ignored
# W: write only, reads are undefined, writes affect peripheral
# R/W: read and write, reads do not affect the devices, and writes can affect the peripheral
# RC: read to clear, reading sets the field to 0, writes are ignored
# RS: read to set, reading sets the field to 1, writes are ignored
# R/WO: read/write 0, can read, but only write 0, writing 1 does not do anything
# R/W1: read/write 1, can read, but only write 1, writing 0 does not do anything
# R/W1C: read/write 1 to clear, can read, but writing 1 will clear the field to 0, writing 1 does not do anything
# R/W1S: read/write 1 to set, can read, writing 1 works, but writing 0 does not do anything
class MAX78000_UART_MemoryRegion(MemoryRegion):
    def __init__(self, start=0, size=0, mmio=False, name='???', use_concrete=True):
        super().__init__(start, size, mmio, name, use_concrete)
        self.register_offsets = {
            0x00: ('CTRL', self.__CTRL_read, self.__CTRL_write, 1 << 24),
            0x04: ('STATUS', self.__STATUS_read, self.__STATUS_write, 0b101 << 27),
            0x08: ('INT_EN', self.__INT_EN_read, self.__INT_EN_write, 0),
            0x0C: ('INT_FL', self.__INT_FL_read, self.__INT_FL_write, 0),
            0x10: ('CLKDIV', self.__CLKDIV_read, self.__CLKDIV_write, 0),
            0x14: ('OSR', self.__OSR_read, self.__OSR_write, 0),
            0x18: ('TXPEEK', self.__TXPEEK_read, self.__TXPEEK_write, 0),
            0x1C: ('PNR', self.__PNR_read, self.__PNR_write, 0b11),
            0x20: ('FIFO', self.__FIFO_read, self.__FIFO_write, 0),
            0x30: ('DMA', self.__DMA_read, self.__DMA_write, 0),
            0x34: ('WKEN', self.__WKEN_read, self.__WKEN_write, 0),
            0x38: ('WKFL', self.__WKFL_read, self.__WKFL_write, 0),
        }
        self.read_write_hook_active = False
        self.sent = b''  # sent to the device, UART -> DEVICE
        self.received = b''  # received from the device, DEVICE -> UART
    
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
        state.memory.store(addr, data, endness='little')
        self.read_write_hook_active = False
    
    # control flags
    def __CTRL_read(self, state, addr, length):
        # bit (name access): purpose
        # 0-3 (rx_thd_val R/W): RX_FIFO depth, how large the receive FIFO is from 1-8, set to 0b1000
        # 4 (par_en R/W): 0 is disabled, 1 is enabled; set to 0
        # 5 (par_eo R/W): 0 is even, 1 is odd; set to 0
        # 6 (par_md R/W): 0 is mark, 1 is space; set to 0
        # 7 (cts_dis R/W): 0 enabled, 1 disabled; set to 1
        # 8 (tx_flush W1): always read as 0, but iff 1 is written, then flush the TX FIFO 
        # 9 (rx_flush W1): always read as 0, but iff 1 is written, then flush the RX FIFO
        # 10 (char_size R/W): values + 5 = char_size; set to 0
        # 12 (stop_bits R/W): 0 is 1, 1 is 1.5; set to 0
        # 13 (hfc_en R/W): hardware flow control enable, set to 0
        # 14 (rtsdc R): idk, but set to 0
        # 15 (bclken R/W): baud clock enable, set to 0
        # 16-17 (bclksrc R/W): baud clock source; corresponds to MAX78000 clock options, set to 0
        # 18 (dpfe_en R/W): parity frame error detection, set to 0
        # 19 (bclkrdy R): baud clock ready, set to 1
        # 20 (ucagm R/W): UART clock auto gating???, set to 1, software needs to set this to 1 before everything works.
        # 21 (fdm R/W): Low Power UART only, set to 0
        # 22 (desm R/W): Low Power UART only, set to 0
        # 23-31 (- DNM): reserved, set to 0
        out = 0 & (0b10000001 << 24)
        out = out & 0b11 << 3
        self.__write_state(state, addr, out)
        print("read of control flags")

    def __CTRL_write(self, state, addr, data):
        print(f"write to control flags @ 0x{addr:x} with 0b{data:b}")

    # status register
    def __STATUS_read(self, state, addr, length):
        # 0 (tx_busy RO): 
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
