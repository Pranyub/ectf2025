import pwn

my_elf = ELF("/home/arjunkha/ectf2025/prectf/task1/main")


for cur_address in list(my_elf.search("")):                                                                                                                         
    my_elf.write(cur_address, "P0wn3dB0t")                                                                                
