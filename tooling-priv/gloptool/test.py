from glop import *
import angr
import archinfo
import IPython

p = PicoTarget('../../prectf/task2-1/build/test.elf')
f = FrameshiftSolver(p)

f.annotate_instrs()
IPython.embed()