from glop import *
import angr
import archinfo
import IPython

h = None
h = HardwareTarget(port=3330)
p = PicoTarget('../../prectf/task2-1/build/test.elf', concrete_target=h)
f = FrameshiftSolver(p)
#f.add_fn_hooks([])
f.annotate_instrs()
f.run()
IPython.embed()