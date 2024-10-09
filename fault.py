from pwn import *
def run_with_glitch(progname: str, glitches: list) -> None:
    task1elf = ELF(progname)
    for cur_address, instr_size in glitches:
        task1elf.write(cur_address, b'\x90' * instr_size)
    task1elf.save(progname+"Modified")
    p = process(progname+"Modified")
    p.interactive()

run_with_glitch("/home/nyub/ectf2025/prectf/task1/out/task1", [(0x400001dc, 6)])