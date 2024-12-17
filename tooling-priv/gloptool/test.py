from glop import *
import angr
import archinfo
import IPython

# h = HardwareTarget(port=3330, config='gdb')
p = MAX78000Target('./binaries/MAX/riscv.elf')
f = FrameshiftSolver(p)
#f.add_fn_hooks([])
f.annotate_instrs()
f.run()
IPython.embed()