/* linker.ld */

SECTIONS
{
    /* Define the text section starting at 0x40000000 */
    .text 0x40000000 : 
    {
        *(.text)     /* All text (code) sections */
    }

    /* Define the init_array and fini_array for C runtime initialization */
    .init_array :
    {
        __init_array_start = .;
        KEEP(*(.init_array))
        __init_array_end = .;
    }

    .fini_array :
    {
        __fini_array_start = .;
        KEEP(*(.fini_array))
        __fini_array_end = .;
    }

    /* Define the data section, if needed */
    .data : 
    {
        *(.data)     /* All data sections */
    }

    /* Define the bss section, if needed */
    .bss : 
    {
        *(.bss)      /* All uninitialized data (bss) */
    }

    /* Define the stack section starting at 0x2000000 */
    .stack 0x2000000 :
    {
        _stack_start = .;  /* Stack start */
        . = . + 0x10000;   /* Define stack size (e.g., 64KB) */
        _stack_end = .;    /* Stack end */
    }
}

/* Provide entry symbol to set the stack pointer */
PROVIDE(_stack = _stack_end);