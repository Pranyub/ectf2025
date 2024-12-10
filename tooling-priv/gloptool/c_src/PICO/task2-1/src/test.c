#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "example.h"

int main() {
    stdio_init_all(); // DO NOT REMOVE. All user code should be after this
    printf("Hello World!");
    volatile int foo = 1;
    while (foo) {}
}