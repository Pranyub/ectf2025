cmake_minimum_required(VERSION 3.13)

# initialize the SDK directly
include(../pico_sdk_import.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

add_executable(hello_serial
        hello_serial.c
        )

enable_language( C CXX ASM )

# pull in common dependencies
target_link_libraries(hello_serial pico_stdlib)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(hello_serial)
