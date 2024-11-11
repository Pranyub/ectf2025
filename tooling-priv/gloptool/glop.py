from targets import PicoTarget, HardwareTarget
import angr
import pyvex
import archinfo
import capstone

# Engine to find glitch gadgets for a target binary
class ConcolicEngine:
    def __init__(self):
        pass

# Generic glitch gadget search engine instance.
# Given a specific exit criteria, tries to induce glitches until the criteria is met
class GadgetSolver:

    def __init__(self, target):
        self.glitch_targets = []
        self.failed = []            # addresses of instructions that aren't vulnerable to glitches
        self.success = []           # addresses of instructions that are vulnerable to glitches
        self.target = target        # Target class

    # Adds a new glitch to the state
    def attempt_glitch(self, state):
        pass

    # Returns true if the current block has an instruction that may be a vulnerable glitch target
    def criteria_hint(self, state):
        return False

    # Returns true if the current state meets the exit criteria, false otherwise
    # Default exit criteria is if $pc is symbolic
    def criteria_met(self, state):
        return False

    # Returns true if function is possibly *interesting*
    def function_of_interest(self, func) -> bool:
        return False

    # Returns list of all function addresses starting at entry_addr to a certain depth
    def trace_function(self, entry_addr=None, depth=3):

        if depth < 1:
            return []

        t = self.target
        cfg = t.cfg

        if not entry_addr:
            entry_addr = cfg.kb.functions.get('main').addr

        entry = t.proj.kb.functions.get(entry_addr)

        ret = []
        ret.append(entry_addr)

        for address in entry.get_call_sites():
            ret += list(self.trace_function(entry_addr=entry.get_call_target(address), depth=depth-1))

        return set(ret)

    # Annotates functions
    def annotate_instrs(self):
        funcs = self.trace_function()
        t = self.target
        for addr in funcs:
            func = t.proj.kb.functions.get(addr)
            if self.function_of_interest(func):
                print(f'candidate: {func.name}')

    def nop_hook(self, state):
        print('nop hook!!')
        state.regs.pc = state.regs.lr
    
    def zero_hook(self, state):
        state.regs.r0 = 0
        return self.nop_hook(state)


# Solver for stack frame shift category of problems
class FrameshiftSolver(GadgetSolver):
    def __init__(self, target):
        super().__init__(target)
        self.glitch_targets = {
            capstone.arm.ARM_INS_PUSH,
            capstone.arm.ARM_INS_POP,
        }

    def add_fn_hooks(self, hooks):
        project = self.target.proj
        for func_addr in self.trace_function():
            print(f'hooking {hex(func_addr)}')
            project.hook(func_addr, self.nop_hook, length=4)

    def function_of_interest(self, func) -> bool:
        for block in func.blocks:
            for insn in block.capstone.insns:
                if insn.id in self.glitch_targets:
                    return True
        return False

    def run(self):
        project = self.target.proj
        self.target.entry.regs.pc = self.target.cfg.kb.functions.get('main').addr
        self.target.entry.regs.sp = self.target.initial_sp

        self.simgr = project.factory.simgr(self.target.entry)
        self.simgr.run()
        #while len(self.simgr.active) == 1:
        #    self.simgr.step()
