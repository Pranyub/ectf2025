import claripy
import time

from peripherals.MemoryRegion import MemoryRegion

# https://developer.arm.com/documentation/dui0552/a/cortex-m3-peripherals/system-timer--systick?lang=en
class MAX78000_SYST_MemoryRegion(MemoryRegion):
    def __init__(self, start=0, size=0, mmio=False, name='???', use_concrete=True):
        super().__init__(start, size, mmio, name, use_concrete)
        self.register_offsets = {
            0x00: ("CSR", self.__CSR_read, self.__CSR_write, 4),  # assume processor clock
            0x04: ("RVR", self.__RVR_read, self.__RVR_write, 0),  # unknown reset
            0x08: ("CVR", self.__CVR_read, self.__CVR_write, 0),  # unknown reset
            0x0C: ("CALIB", self.__CALIB_read, self.__CALIB_write, 0)  # READ ONLY REGISTER
        }
        self.read_write_hook_active = False
        self.enabled = False
        self.reload = 0
        self.start_time = time.time_ns()
        self.clock_speed = 100 * (10 ** 6)  # 100MHz

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
            # print(f"Read of {self.name}_{self.register_offsets[offset][0]} register")
            self.register_offsets[offset][1](state, addr)
        else:
            print(f"WARNING: read of unknown {self.name} register at address 0x{addr:x} with an offset of 0x{offset:x} !")
    
    def write(self, state, addr, data):
        if self.read_write_hook_active:
            return
        offset = addr - self.base
        if offset in self.register_offsets:
            # print(f"Write to {self.name}_{self.register_offsets[offset][0]} register: 0x{data:x}")
            self.register_offsets[offset][2](state, addr, data)
        else:
            print(f"WARNING: write to unknown {self.name} register at address 0x{addr:x}: 0x{data:x} !")
    
    def __CSR_read(self, state, addr):
        # 31-17 (- ?): reserved
        # 16 (countflag ?): returns 1 if counter has reached 0 since the last read
        # 15-3 (- ?): reserved
        # 2 (clksource ?): 0 for external, 1 for internal
        # 1 (tickint ?): 0 no interrupts, 1 for interrupts
        # 0 (enable ?): 0 disabled, 1 enabled
        # assume no interrupts
        down = int((time.time_ns() - self.start_time) >= (((10 ** 9) / self.clock_speed) * self.reload))
        if self.enabled:
            down = 0
        out = down << 16  # replace
        out = out | (1 << 2)
        out = out | int(self.enabled)
        self.__write_state(state, addr, out)

    def __CSR_write(self, state, addr, data):
        self.enabled = (data & 1) == 1
        if self.enabled:
            self.start_time = time.time_ns()

    def __RVR_read(self, state, addr):
        # 31-24 (- RO): reserved
        # 23-0 (reload ?): value to load into the register when it reaches 0
        self.__write_state(state, addr, self.reload)

    def __RVR_write(self, state, addr, data):
        self.reload = data & 0x00FFFFFF
        self.start_time = time.time_ns()

    def __CVR_read(self, state, addr):
        # 31-24 (- RO): reserved
        # 23-0 (current ?): current systick counter. writes will clear to 0 and set currentflag to 0
        if not self.enabled:
            self.__write_state(state, addr, 0)
            return
        out = ((self.start_time - time.time_ns()) // ((10 ** 9) // self.clock_speed)) % self.reload  # replace
        self.__write_state(state, addr, out)

    def __CVR_write(self, state, addr, data):
        self.start_time = time.time_ns()
        self.__write_state(state, addr, 0)

    def __CALIB_read(self, state, addr):
        # 31 (noref R): 0 reference clock, 1 no reference clock.  set to 1
        # 30 (skew R): 0 TENMS is accuarte, 1 TENMS is not accurate.  set to 0
        # 29-24 (- RO): reserved
        # 23-0 (tenms R): reload value for 100hz timing.  0 if unknown.  Set to 1000000
        out = 1 << 31
        out = out | self.clock_speed // 100
        self.__write_state(state, addr, out)

    def __CALIB_write(self, state, addr, data):
        print(f"WARNING: {self.name}_CALIB has been written to.  This is read-only memory.")
