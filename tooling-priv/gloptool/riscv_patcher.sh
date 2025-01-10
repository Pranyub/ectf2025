#!/bin/sh
if [ $# -ne 2 ]; then
    echo "Usage: $0 input_elf ouput_elf"
    exit 1
fi

if [ "$1" = "--help" ]; then
    echo "Provide an input elf, and a patched copy will be created"
    exit 0
fi

addr_in_hex="$(readelf -S $1 | grep .riscv_flash | tr -s ' ' | tr ' ' '\n' | sed -n '5p')"

if ! [ -n "$addr_in_hex" ]; then
    echo "Couldn't parse elf file"
    exit 1
fi

objcopy -I elf32-little --remove-section=.riscv_flash "$1" "$2"

if [ $? -ne 0 ]; then
    echo "Couldn't remove .riscv_flash section"
    exit 1
fi

# temp file
truncate -s 324 nullbytes

if [ $? -ne 0 ]; then
    echo "Couldn't create nullbytes file"
    exit 1
fi

objcopy -I elf32-little --add-section .riscv_flash=nullbytes "$2"
rm nullbytes

if [ $? -ne 0 ]; then
    echo "Couldn't add .riscv_flash section"
    exit 1
fi

# convert hex address to dec
address="$(printf '%d' 0x$addr_in_hex)"

objcopy -I elf32-little --change-section-address .riscv_flash="$address" "$2"

if [ $? -ne 0 ]; then
    echo "Couldn't change .riscv_flash section address"
    exit 1
fi

echo "Success"

exit 0
