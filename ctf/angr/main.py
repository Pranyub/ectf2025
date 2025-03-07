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
            print("Found a crash")
            return True
        return False
    
def run_until(simgr):
    return simgr.errored or simgr.found


# returns the constraints to be added and the input
def simple_opcode_packets():
    magic = claripy.BVV(b'%')
    length = claripy.BVS('opcode', 8)
    max_length = 64
    body = claripy.BVS('content', 8 * max_length)
    input = claripy.Concat(magic, length, body)
    return [claripy.ULT(length, max_length + 1)], input

def main():
    proj = angr.Project('../decoder/decoder')
    constraints, input = simple_opcode_packets()
    state = proj.factory.entry_state(stdin=angr.SimFileStream('stdin', content=input))
    for constraint in constraints:
        state.solver.add(constraint)
    simgr = proj.factory.simulation_manager(state)
    simgr.stashes.setdefault('found', [])
    # simgr.use_technique(SearchForCrash())
    # simgr.use_technique(SearchForInsane())
    simgr.run(until=run_until)

    if simgr.found:
        crash_state = simgr.found[0]
        print("Crash found!")
        print("Input causing crash:", crash_state.posix.dumps(0))
    else:
        print("Found nothing.")

    if simgr.errored:
        for error in simgr.errored:
            state, exc = error
            print("Crash found: ")
            print("\tState:", state)
            print("\tInput:", repr(simgr.posix.dumps(0)))
    

main()

