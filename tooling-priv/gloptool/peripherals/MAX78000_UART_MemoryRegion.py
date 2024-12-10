import claripy

from .MemoryRegion import MemoryRegion

# https://www.analog.com/media/en/technical-documentation/user-guides/max78000-user-guide.pdf
# memory access meanings:
# RO: reserved, reads reset the field to the default value, writes are ignored
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
            0x00: ('CTRL', self.__CTRL_read, self.__CTRL_write, 1 << 7),
            0x04: ('STATUS', self.__STATUS_read, self.__STATUS_write, 0b101 << 4),
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
        self.sending = "Hello, world from UART!"  # only used for debugging the FIFO reads
    
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
    
    # control flags
    def __CTRL_read(self, state, addr):
        # bit (name access): purpose
        # 31-23 (- DNM): reserved, set to 0
        # 22 (desm R/W): Low Power UART only, set to 0
        # 21 (fdm R/W): Low Power UART only, set to 0
        # 20 (ucagm R/W): UART clock auto gating???, set to 1, software needs to set this to 1 before everything works.
        # 19 (bclkrdy R): baud clock ready, set to 1
        # 18 (dpfe_en R/W): parity frame error detection, set to 0
        # 16-17 (bclksrc R/W): baud clock source; corresponds to MAX78000 clock options, set to 0
        # 15 (bclken R/W): baud clock enable, set to 0
        # 14 (rtsdc R): idk, but set to 0
        # 13 (hfc_en R/W): hardware flow control enable, set to 0
        # 12 (stop_bits R/W): 0 is 1, 1 is 1.5; set to 0
        # 11-10 (char_size R/W): values + 5 = char_size; set to 0
        # 9 (rx_flush W1): always read as 0, but iff 1 is written, then flush the RX FIFO
        # 8 (tx_flush W1): always read as 0, but iff 1 is written, then flush the TX FIFO 
        # 7 (cts_dis R/W): 0 enabled, 1 disabled; set to 1
        # 6 (par_md R/W): 0 is mark, 1 is space; set to 0
        # 5 (par_eo R/W): 0 is even, 1 is odd; set to 0
        # 4 (par_en R/W): 0 is disabled, 1 is enabled; set to 0
        # 3-0 (rx_thd_val R/W): RX_FIFO depth, how large the receive FIFO is from 1-8, set to 0b1000??
        out = 0b11 << 19
        out = out & 0b10001000
        self.__write_state(state, addr, out)

    def __CTRL_write(self, state, addr, data):
        pass

    # status register
    def __STATUS_read(self, state, addr):
        # 31-16 (- RO): reserved, set to 0
        # 15-12 (tx_lvl RO): transmit FIFO level, number of characters in the transmit FIFO, set to 0
        # 11-8 (rx_lvl RO): receive FIFO level, number of characters in the recieve FIFO, set to 0
        # 7 (tx_full RO): transmit FIFO full, set to 0
        # 6 (tx_em RO): transmit FIFO empty, set to 1
        # 5 (rx_full RO): receive FIFO full, set to 0
        # 4 (rx_em RO): receive FIFO empty, set to 1
        # 3-2 (- RO): reserved, set to 0
        # 1 (rx_busy RO): recieve busy, 0 not receiving, 1 sending, set to 0??
        # 0 (tx_busy RO): transmit busy, 0 not transmitting, 1 transmitting, set to 0??
        out = 0b101000
        self.__write_state(state, addr, out)

    def __STATUS_write(self, state, addr, data):
        # this should really never happen
        pass

    def __INT_EN_read(self, state, addr):
        # hopefully interrupts aren't used
        pass

    def __INT_EN_write(self, state, addr, data):
        pass

    def __INT_FL_read(self, state, addr):
        # hopefully interrupts aren't used
        pass

    def __INT_FL_write(self, state, addr, data):
        pass
    
    def __CLKDIV_read(self, state, addr):
        # might not matter
        pass

    def __CLKDIV_write(self, state, addr, data):
        pass

    def __OSR_read(self, state, addr):
        # not needed
        pass

    def __OSR_write(self, state, addr, data):
        pass

    def __TXPEEK_read(self, state, addr):
        self.__write_state(state, addr, self.register_offsets[addr][3])

    def __TXPEEK_write(self, state, addr, data):
        self.__write_state(state, addr, self.register_offsets[addr][3])

    def __PNR_read(self, state, addr):
        # not needed
        pass

    def __PNR_write(self, state, addr, data):
        pass

    def __FIFO_read(self, state, addr):
        # 31-9 (- RO): reserved
        # 8 (rx_par R): byte parity, set to 0
        # 7-0 (data R/W): reading reads the next character in the FIFO, writing adds a character to the FIFO
        out = ord(self.sending[0])
        if len(sending) > 0:
            sending = sending[1:]
        self.sent += out.to_bytes(1, 'big')
        self.__write_state(state, addr, out)
        print(f"{id(self)} UART -> MAX78000: {self.sent.decode('utf-8')}")

    def __FIFO_write(self, state, addr, data):
        self.received += data.to_bytes(1, 'big')
        print(f"{id(self)} MAX78000 -> UART: {self.received.decode('utf-8')}")
    
    def __DMA_read(self, state, addr):
        # assume DMA off
        pass
        
    def __DMA_write(self, state, addr, data):
        pass

    def __WKEN_read(self, state, addr):
        # assume no wakeup
        pass

    def __WKEN_write(self, state, addr, data):
        pass
    
    def __WKFL_read(self, state, addr):
        # assume no wakeup
        pass

    def __WKFL_write(self, state, addr, data):
        pass

    def read(self, state, addr, size=4):
        if self.read_write_hook_active:
            return
        assert size == 4
        offset = addr - self.base
        if offset in self.register_offsets:
            print(f"Read of UART{self.register_offsets[offset][0]} register")
            self.register_offsets[offset][1](state, addr)
        else:
            print(f"Read of unknown UART register at address 0x{addr:x} with an offset of 0x{offset:x} !!!!!!!")
    
    def write(self, state, addr, data):
        if self.read_write_hook_active:
            return
        offset = addr - self.base
        if offset in self.register_offsets:
            print(f"Write to UART{self.register_offsets[offset][0]} register: 0x{data:x}")
            self.register_offsets[offset][2](state, addr, data)
        else:
            print(f"Write to unknown UART register at address 0x{addr:x}: 0x{data:x} !!!!!!!!!!!!!!!!!!!!!!!!")