import os
import claripy
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import padding

from .MemoryRegion import MemoryRegion

class MAX78000_AES_MemoryRegion(MemoryRegion):
    def __init__(self, start=0, size=0, mmio=False, name='???', use_concrete=True):
        super().__init__(start, size, mmio, name, use_concrete)
        self.register_offsets = {
            0x0000: ("CTRL", self.__CTRL_read, self.__CTRL_write, 0),
            0x0004: ("STATUS", self.__STATUS_read, self.__STATUS_write, 0),
            0x0008: ("INTFL", self.__INTFL_read, self.__INTFL_write, 0),
            0x000C: ("INTEN", self.__INTEN_read, self.__INTEN_write, 0),
            0x0010: ("FIFO", self.__FIFO_read, self.__FIFO_write, 0),
            0x0400: ("KEY", self.__KEY_read, self.__KEY_write, 0)
        }
        self.read_write_hook_active = False
        self.encryption_size = 0  # multiply this value by 64 and add 128 to get # of bits
        self.encryption_type = 0
        self.input = b''
        self.output = b''
        self.internal_key = b''
        self.cipher = None

    def __AES_encrypt(self, state):
        key = b''
        if self.encryption_type < 2:
            key = self.__AES_get_external_key(state)
            if int.from_bytes(key, 'little') == 0:  # if there is no key, and we're trying to use an external key, just use the local key.  We're only doing this because the documenation is unclear about the AES process
                if len(self.internal_key) != 16 + self.encryption_size * 8:
                    self.internal_key = os.urandom(16 + self.encryption_size * 8)
                key = self.internal_key
        else:
            if len(self.internal_key) != 16 + self.encryption_size * 8:  # not sure if this is how it should go or not
                self.internal_key = os.urandom(16 + self.encryption_size * 8)
            key = self.internal_key

        padded = self.input
        if len(self.input) < 16 + self.encryption_size * 8:
            padder = padding.PKCS7(128 + self.encryption_size * 64).padder()  # documentation does not say what type of padding is used
            padded = padder.update(self.input)
            padded += padder.finalize()
        print(f"AES_Encrypt Padded PT: 0x{padded.hex()}")
        cipher = Cipher(algorithms.AES(key), modes.ECB())
        encryptor = cipher.encryptor()
        buf = bytearray(len(padded) + 16 + self.encryption_size * 8 - 1)
        len_encrypted = encryptor.update_into(padded, buf)
        ct = bytes(buf[:len_encrypted]) + encryptor.finalize()
        print(f"AES_Encrypt CT: 0x{ct.hex()}")
        return ct    

    def __AES_decrypt(self, state):
        key = b''
        if self.encryption_type < 2:
            key = self.__AES_get_external_key(state)
        else:
            key = self.internal_key

        cipher = Cipher(algorithms.AES(key), modes.ECB())
        decryptor = cipher.decryptor()
        print(f"AES_Decrypt CT: 0x{self.input.hex()}")
        buf = bytearray(len(self.input) + 16 + self.encryption_size * 8 - 1)
        len_decrypted = decryptor.update_into(self.input, buf)
        padded = bytes(buf[:len_decrypted]) + decryptor.finalize()
        print(f"AES_Decrypt Padded PT: 0x{padded.hex()}")
        unpadder = padding.PKCS7(128 + self.encryption_size * 64).unpadder()
        unpadded = unpadder.update(padded)
        unpadded += unpadder.finalize()
        return unpadded

    def __AES_get_external_key(self, state):
        in_register = self.__read_state(state, self.base + 0x0400, 16 + self.encryption_size * 8)
        return in_register.to_bytes(16 + self.encryption_size * 8, 'little')

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
        # 31-10 (- RO): reserved
        # 9-8 (type R/W): encryption type,
        #   00 encryption using external key,
        #   01 decryption using external key,
        #   10 decryption using locally generated key,
        #   11 reserved
        # 7-6 (key_size R/W): encryption key size,
        #   00 128-bit
        #   01 192-bit
        #   10 256-bit
        #   11 reserved
        # 5 (output_flush W1): flush data output FIFO, 1 write flush. set to 0
        # 4 (input_flush W1): flush data input FIFO, 1 write flush. set to 0
        # 3 (start W1): start AES calculation, force calculation to start, even if the FIFO is not full
        # 2 (dma_tx_en R/W): generate DMA request when data input FIFO is empty, set to 0
        # 1 (dma_rx_en R/W): generate DMA request when data output FIFO is full, set to 0
        # 0 (en R/W): AES enable, set to 1
        # NOTE: the documentation does not discuss where the external/local key is stored
        out = self.encryption_type << 8
        out = out | (self.encryption_size << 6)
        out = out | 1
        self.__write_state(state, addr, out)

    def __CTRL_write(self, state, addr, data):
        self.encryption_type = ((data & (0b11 << 8)) >> 8)
        self.encryption_size = ((data & (0b11 << 6)) >> 6)
        if data & (1 << 5) != 0:
            self.output = b''
        if data & (1 << 4) != 0:
            self.input = b''
        if data & (1 << 3) != 0:
            # should really only be happenening for encryptions, but we'll add the decryption case in case
            if self.encryption_type == 0:
                self.output = self.__AES_encrypt(state)
            else:
                self.output = self.__AES_decrypt(state)
            self.input = b''

    def __STATUS_read(self, state, addr):
        # 31-5 (- RO): reserved
        # 4 (output_full R): output FIFO full, 0 not full, 1 full
        # 3 (output_em R): output FIFO empty, 0 not empty, 1 empty
        # 2 (input_full R): input FIFO full, 0 not full, 1 full
        # 1 (input_em R): input FIFO empty, 0 not empty, 1 empty
        # 0 (busy R): AES busy, set to 0
        out = int(len(self.output) >= (16 + self.encryption_size * 8)) << 4
        out = out | (int(len(self.output) == 0) << 3)
        out = out | (int(len(self.input) >= (16 + self.encryption_size * 8)) << 2)
        out = out | (int(len(self.input) == 0) << 1)
        self.__write_state(state, addr, out)

    def __STATUS_write(self, state, addr, data):
        # should never happen
        pass

    def __INTFL_read(self, state, addr):
        # 31-4 (- RO): reserved
        # 3 (ov W1C): data output FIFO overrun event interrupt, 0 no event, 1 event occured.  Set to 0
        # 2 (key_zero W1C): key zero event interrupt, 0 no event, 1 event occured
        # 1 (key_change W1C): key change event interrupt, 0 no event, 1 event occured
        # 0 (done W1C): calculation done event, 0 no event, 1 event occured, set to 1
        # assume interrupts are disabled, but keep done event always on
        self.__write_state(state, addr, 1)

    def __INTFL_write(self, state, addr, data):
        pass

    def __INTEN_read(self, state, addr):
        # 31-4 (- RO): reserved
        # 3 (ov W1C): data output FIFO overrun event interrupt enable, 0 enabled, 1 disabled. set to 1
        # 2 (key_zero W1C): key zero event interrupt enable, 0 enabled, 1 disabled. set to 1
        # 1 (key_change W1C): key change event intterupt, 0 enabled, 1 disabled. set to 1
        # 0 (done W1C): calculation done event interrupt, 0 enabled, 1 disabled. set to 1
        # assuming interrupts are disabled
        self.__write_state(state, addr, 0b1111)

    def __INTEN_write(self, state, addr, data):
        print(f"{self.name} device attempted to enable interrupts")
        pass

    def __FIFO_read(self, state, addr):
        # 31-0 (data R/W): AES FIFO, writes put data into the FIFO, reads pull data out of the FIFO (lsb first)
        out = 0
        if len(self.output) != 0:
            # out = self.output[0:4]  # might need to swap so its [-4:] and [:-4]
            # self.output = self.output[4:]
            out = self.output[-4:]
            self.output = self.output[:-4]
        self.__write_state(state, addr, out)
        
    def __FIFO_write(self, state, addr, data):
        if len(self.input) >= (16 + self.encryption_size * 8):
            print(f"DEVICE attmped to write to full {self.name} FIFO")
            return
        self.input = data.to_bytes(4, 'big') + self.input
        if len(self.input) == (16 + self.encryption_size * 8):
            if self.encryption_type == 0:
                self.output = self.__AES_encrypt(state)
            else:
                self.output = self.__AES_decrypt(state)
            self.input = b''

    def __KEY_read(self, state, addr):
        pass

    def __KEY_write(self, state, addr, data):
        pass
        