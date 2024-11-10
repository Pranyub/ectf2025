cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_PATH "/common/pico-sdk") # change path if not on build server
set(PICO_BOARD "pico2")               # change if not targeting rp2350

include(../pico_sdk_import.cmake)

project(test_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

include_directories("inc/")

file(GLOB test_SRC CONFIGURE_DEPENDS "inc/*.h" "src/*.c")

add_executable(test
  ${test_SRC}
)

pico_enable_stdio_usb(test 1)
pico_enable_stdio_uart(test 1)

pico_add_extra_outputs(test)

target_link_libraries(test PRIVATE pico_stdlib)
