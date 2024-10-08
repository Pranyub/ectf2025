from pwn import *
def run_with_glitch(progname: str, glitches: list) -> None:
    task1elf = ELF(progname)
    for cur_address in glitches:
        task1elf.asm(task1elf.symbols['main'] + cur_address, 'nop')
    task1elf.save(progname+"Modified")
    p = process(progname+"Modified")
    p.interactive()

run_with_glitch("/home/adityanadarajah/Desktop/CTF/ectf2025-main(1)/ectf2025-main/prectf/task1/out/task1", [0xd7, 0xdc, 0xe1])