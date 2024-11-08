from glop import *
import angr
import archinfo
import IPython

h = HardwareTarget(port=3330)
p = PicoTarget('./test.elf', concrete_target=h)
f = FrameshiftSolver(p)
f.add_fn_hooks([])
f.annotate_instrs()
f.run_concrete()
IPython.embed()