#!/bin/python
import angr
import logging
import claripy

# logging.getLogger('angr').setLevel('DEBUG')

class SearchForCrash(angr.ExplorationTechnique):
    def setup(self, simgr):
        if 'found' not in simgr.stashes:
            simgr.stashes.setdefault('found', [])

    def filter(self, simgr, state, **kwargs):
        if state.history.jumpkind in ('Ijk_SigSEGV', 'Ijk_SigILL', 'Ijk_SigFPE', 'Ijk_SigBUS', 'Ijk_Exception'):
            simgr.stashes['found'].append(state)
            return False
        return True

def run_until(simgr):
    return simgr.errored

def main():
    proj = angr.Project('../decoder/decoder')
    input_size = 100
    inp = claripy.Concat(claripy.BVV(b'%'), claripy.BVS('input', 8 * input_size))
    state = proj.factory.entry_state(stdin=angr.SimFileStream('stdin', content=inp))
    simgr = proj.factory.simulation_manager(state)
    # simgr.use_technique(SearchForCrash())
    # simgr.use_technique(SearchForInsane())
    simgr.run(until=run_until)

    # if simgr.found:
    #     crash_state = simgr.found[0]
    #     print("Crash found!")
    #     print("Input causing crash:", crash_state.posix.dumps(0))
    # else:
    #     print("No crash found.")

    if simgr.errored:
        for error in simgr.errored:
            state, exc = error
            print("Crash found: ")
            print("\tState:", state)
            print("\tInput:", repr(simgr.posix.dumps(0)))

    

main()

