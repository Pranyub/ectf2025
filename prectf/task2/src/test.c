#include <stdio.h>
#include <unistd.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "example.h"

#define MAX_COMMAND_BUFFER 16

const uint LED_PIN = 25;

int main() {
    bi_decl(bi_program_description("This is a test binary."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    stdio_init_all();  // IMPORTANT

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1) {
        // gpio_put(LED_PIN, 0);
        // sleep_ms(250);
        // gpio_put(LED_PIN, 1);

        char buffer[MAX_COMMAND_BUFFER] = "";        
        if (read(0, buffer, MAX_COMMAND_BUFFER) != MAX_COMMAND_BUFFER) {
            fprintf(stderr, "Error reading.");
            return 1;
        }
        printf("You put: %s", buffer);
    }
}