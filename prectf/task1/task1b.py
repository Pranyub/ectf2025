# Pwntools is a framework written in python that provides us with many useful functions for         
# manipulating binaries, connecting to various services, and writing exploits. It is perfect for    
# writing exploit POC's and CTF's.                                                                  

# In this example, we're leveraging pwntools binary patching abilities to modify an IRC bot.        
# We're not doing anything particularly complicated here, just opening the ELF, searching for a     
# string, and replacing it with another.                                                            
 
from pwn import *                                                                                   

def replace_section(filename, searchString):
    # First, load the ELF.                                                                              
    my_elf = ELF(filename)                                                                                

    for cur_address in list(my_elf.search(searchString)):                                                
        # Notice that the names are the same length... We don't want to mess up our offsets or overwrite
        # other data here.                                                                              
        my_elf.write(cur_address, b'\x90' * len(searchString))                                                          
    #end for                                                                                            

        my_elf.save("ch31_modified") 

def replace_address(filename, address):
    # First, load the ELF.                                                                              
    my_elf = ELF(filename)                                                                                

                                                                        
    my_elf.write(address, b'\x90')                                                          
    #end for                                                                                            

    my_elf.save("ch31_modified") 


method = int(input("Enter 1 for searchString and 2 for address: "), 16)
user_input = input("Enter the filename: ")

if(method == 1):
    search = input("Enter the string to search for: ")

    replace_section(user_input, search)
elif method == 2:
    address = int(input("Enter the address: "))

    replace_address(user_input, address)
