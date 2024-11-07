#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/unique_id.h"
#include "example.h"

const uint LED_PIN = 25;

int target() {
    uint8_t buf[16];
    for (int i = 0; i < 32; i++) {
        buf[i] = getchar();
        if (buf[i] == '\n')
            break;
    }
    printf("%s", buf);
    return 0;
}

int main() {
    bi_decl(bi_program_description("This is a test binary."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    stdio_init_all(); // DO NOT REMOVE. All user code should be after this
    target();
    printf("target is %p\n", target);

    while (1) {
        target();
    }
}