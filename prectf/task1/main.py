from pwn import *

'''
@brief Runs the program with a given name, replacing specified instructions with NOPs.
- The program should run in the same terminal as the python script (e.g. use pwntools.interactive())
@param progname the name of the program to run
@param glitches a list of ints specifying addresses in the program to be replaced with NOPs
'''
def run_with_glitch(progname: str, glitches: list) -> None:
    elf = ELF(progname)
    for cur_address in [0, 100, 1000]:
        elf.write(cur_address, "\x90")
    elf.save(f"{progname}_modified")
    p = process(["setarch", "-R", f"{progname}_modified"])
    p.interactive()
