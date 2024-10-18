#include <stdio.h>
// #include <unistd.h>
#include <string.h>  // for memset
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "example.h"

#define MAX_COMMAND_BUFFER 13  // string length capped at 12, extra for null

const uint LED_PIN = 25;

int main() {
    bi_decl(bi_program_description("This is a test binary."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    stdio_init_all();  // IMPORTANT

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    // sleep_ms(250);  // makes sure stdio_init_all has finished
    while (1) {
        char command_buffer[MAX_COMMAND_BUFFER] = "";
        int command_length = 0;

        // clear stdin
        // parse input for command
        for (char c = getchar(); c != '\n' && c != EOF; c = getchar(), command_length++) {
            command_buffer[command_length] = c;
            printf("%c", c);
            if (command_length >= MAX_COMMAND_BUFFER) {
                break;
            }
        }
        command_buffer[command_length] = '\0';  // terminate input
        printf("You said: %s\n", command_buffer);

        // reset the command
        memset(command_buffer, '\0', MAX_COMMAND_BUFFER);  // not needed
        command_length = 0;
        printf("Starting over again\n");
    }
}