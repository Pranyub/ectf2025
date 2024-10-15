# Pwntools is a framework written in python that provides us with many useful functions for         
# manipulating binaries, connecting to various services, and writing exploits. It is perfect for    
# writing exploit POC's and CTF's.                                                                  

# In this example, we're leveraging pwntools binary patching abilities to modify an IRC bot.        
# We're not doing anything particularly complicated here, just opening the ELF, searching for a     
# string, and replacing it with another.
 
from pwn import *                                                               

def run_with_glitch(progname: str, glitches: list):
    my_elf = ELF(progname)

    for gli in glitches:                                                
        # Notice that the names are the same length... We don't want to mess up our offsets or overwrite
        # other data here.
        my_elf.write(gli, b'\x90' * 6)
    
    p = my_elf.process(["setarch -R"])

    p.interactive()

run_with_glitch("./out/task1", [0x4000015b])

    # #end for                                                                                            

    # my_elf.save("ch31_modified")