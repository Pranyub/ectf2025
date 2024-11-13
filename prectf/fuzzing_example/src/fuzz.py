#!/usr/bin/env python3

import argparse
import os
import math
import signal
import ctypes

from unicornafl import *
from unicorn.arm_const import *
from pwn import *

# Path to the file containing the binary to emulate
BINARY_FILE = '/home/pyondan/ectf2025/prectf/fuzzing_example/build/test.elf'

MAIN_CALL_WIFI_PROCESS_RESPONSE = 0x1000032e
MAIN_END = 0x10000338

WIFI_PROCESS_RESPONSE = 0x100002ac

CODE_ADDRESS = 0x10000000
STACK_ADDRESS = 0x00200000  # Address of the stack (arbitrarily chosen)
STACK_SIZE = 0x00010000  # Size of the stack (arbitrarily chosen)
DATA_ADDRESS = 0x00300000  # Address where mutated data will be placed
DATA_SIZE_MAX = 0x00010000  # Maximum allowable size of mutated data

STDIO_INIT_ALL = 0x100038dc
WIFI_RECV = 0x1000027c
__WRAP_PRINTF = 0x10003ae8

from capstone import *
cs = Cs(CS_ARCH_ARM, CS_MODE_THUMB)

def unicorn_debug_instruction(uc, address, size, user_data):
    mem = uc.mem_read(address, size)
    for (cs_address, cs_size, cs_mnemonic, cs_opstr) in cs.disasm_lite(bytes(mem), size):
        print("    Instr: {:#016x}:\t{}\t{}".format(address, cs_mnemonic, cs_opstr))

        if cs_mnemonic == 'bl' or cs_mnemonic == 'b' or cs_mnemonic == 'b.w' or cs_mnemonic == 'bx':
            if cs_mnemonic == 'bl' or cs_mnemonic == 'b.w':
                # subtract 4 cuz arm thumb pc is weird :/
                fn = address - 4 + ctypes.c_int(int(cs_opstr[1:], 16)).value
                print(f'--- CALL {hex(fn)} ---')

                if fn == STDIO_INIT_ALL or fn == WIFI_RECV:
                    current = uc.reg_read(UC_ARM_REG_PC)
                    print(f'--- RET (skipping function {hex(fn)}) ---')
                    uc.reg_write(UC_ARM_REG_PC, current + 4 + 1)
                if fn == __WRAP_PRINTF:
                    # special case: it ends in "bx lr" so we must ret from current func
                    uc.reg_write(UC_ARM_REG_PC, uc.reg_read(UC_ARM_REG_LR))
                    print('B'*100)
                    print(hex(uc.reg_read(UC_ARM_REG_LR)))
                    print(hex(address))
                    print('B'*100)


        if ('pop' in cs_mnemonic and 'pc' in cs_opstr) or (cs_mnemonic == 'bx' and cs_opstr == 'lr'):
            print('--- RET ---')

def unicorn_debug_block(uc, address, size, user_data):
    print("Basic Block: addr=0x{0:016x}, size=0x{1:016x}".format(address, size))

def unicorn_debug_mem_access(uc, access, address, size, value, user_data):
    if access == UC_MEM_WRITE:
        print("        >>> Write: addr=0x{0:016x} size={1} data=0x{2:016x}".format(address, size, value))
    else:
        print("        >>> Read: addr=0x{0:016x} size={1}".format(address, size))

def unicorn_debug_mem_invalid_access(uc, access, address, size, value, user_data):
    if access == UC_MEM_WRITE_UNMAPPED:
        print("        >>> INVALID Write: addr=0x{0:016x} size={1} data=0x{2:016x}".format(address, size, value))
    else:
        print("        >>> INVALID Read: addr=0x{0:016x} size={1}".format(address, size))

def force_crash(uc_error):
    # This function should be called to indicate to AFL that a crash occurred during emulation.
    # Pass in the exception received from Uc.emu_start()
    mem_errors = [
        UC_ERR_READ_UNMAPPED,
        UC_ERR_READ_PROT,
        UC_ERR_READ_UNALIGNED,
        UC_ERR_WRITE_UNMAPPED,
        UC_ERR_WRITE_PROT,
        UC_ERR_WRITE_UNALIGNED,
        UC_ERR_FETCH_UNMAPPED,
        UC_ERR_FETCH_PROT,
        UC_ERR_FETCH_UNALIGNED,
    ]
    if uc_error.errno in mem_errors:
        # Memory error - throw SIGSEGV
        os.kill(os.getpid(), signal.SIGSEGV)
    elif uc_error.errno == UC_ERR_INSN_INVALID:
        # Invalid instruction - throw SIGILL
        os.kill(os.getpid(), signal.SIGILL)
    else:
        # Not sure what happened - throw SIGABRT
        os.kill(os.getpid(), signal.SIGABRT)

def main():
    parser = argparse.ArgumentParser(description="Fuzz harness for test.elf")
    parser.add_argument(
        "input_file",
        type=str,
        help="Path to the file containing the mutated input to load",
    )
    parser.add_argument(
        "-t",
        "--trace",
        default=False,
        action="store_true",
        help="Enables debug tracing",
    )
    args = parser.parse_args()

    uc = Uc(UC_ARCH_ARM, UC_MODE_THUMB)

    if args.trace:
        # uc.hook_add(UC_HOOK_BLOCK, unicorn_debug_block)
        uc.hook_add(UC_HOOK_CODE, unicorn_debug_instruction)
        uc.hook_add(UC_HOOK_MEM_WRITE | UC_HOOK_MEM_READ, unicorn_debug_mem_access)
        uc.hook_add(UC_HOOK_MEM_WRITE_UNMAPPED | UC_HOOK_MEM_READ_INVALID, unicorn_debug_mem_invalid_access)

    # ---------------------------------------------------
    # Load the binary to emulate and map it into memory

    print("Loading data input from {}".format(args.input_file))
    e = ELF(BINARY_FILE)

    found_segment = False
    for s in e.segments:
        if s.header.p_type == 'PT_LOAD' and s.header.p_vaddr == CODE_ADDRESS:
            found_segment = True

            off = s.header.p_offset
            sz = s.header.p_memsz

            code = s.stream[off:off+sz]

            # just to verify that this is main()
            '''
            main_offset = 0x31c
            for (cs_address, cs_size, cs_mnemonic, cs_opstr) in cs.disasm_lite(code[main_offset:main_offset+16], 16):
                print("    Instr: {:#016x}:\t{}\t{}".format(cs_address, cs_mnemonic, cs_opstr))
            found_segment = True
            '''
            break
    if not found_segment:
        print(f'could not find segment starting at {hex(CODE_ADDRESS)}!')
        return

    # Write the mutated command into the data buffer
    page_round_up = lambda n: math.ceil(n / 4096) * 4096
    uc.mem_map(CODE_ADDRESS, page_round_up(len(code)))
    uc.mem_write(CODE_ADDRESS, code)

    # Set the program counter to the start of the code
    start_address = MAIN_CALL_WIFI_PROCESS_RESPONSE
    end_address = MAIN_END
    uc.reg_write(UC_ARM_REG_PC, start_address+1)

    # -----------------
    # Setup the stack
    uc.mem_map(STACK_ADDRESS, STACK_SIZE)
    uc.reg_write(UC_ARM_REG_SP, STACK_ADDRESS + STACK_SIZE)
    
    # reserve some space for data
    uc.mem_map(DATA_ADDRESS, DATA_SIZE_MAX)
    # set r0 (first arg) to point to data
    uc.reg_write(UC_ARM_REG_R0, DATA_ADDRESS)

    # -----------------------------------------------------
    # Set up a callback to place input data (do little work here, it's called for every single iteration)
    # We did not pass in any data and don't use persistent mode, so we can ignore these params.
    # Be sure to check out the docstrings for the uc.afl_* functions.
    def place_input_callback(uc, input, persistent_round, data):
        # Apply constraints to the mutated input
        if len(input) > DATA_SIZE_MAX:
            # Test input is too long
            return False
        # Write the mutated command into the data buffer
        uc.mem_write(DATA_ADDRESS, input)

    # unicorn_debug_instruction(uc, uc.reg_read(UC_ARM_REG_PC), 10, None)

    # Start the forkserver
    try:
        uc.emu_start(uc.reg_read(UC_ARM_REG_PC)+1, end_address, timeout=0, count=0)
    except UcError as e:
        print("Execution failed with error: {}", e)
        force_crash(e)

    # Start the fuzzer.
    # uc_afl_fuzz(uc=uc, input_file=args.input_file, place_input_callback=place_input_callback, exits=[end_address])

if __name__ == "__main__":
    main()