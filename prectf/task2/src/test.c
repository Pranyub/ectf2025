#include <stdio.h>
// #include <unistd.h>
#include <string.h>  // for memset
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
// #include "pico/pico_mem_ops.h"  // for pico memset
#include "example.h"

#define MAX_COMMAND_BUFFER 13  // string length capped at 12, extra for null

const uint LED_PIN = 25;

int main() {
    bi_decl(bi_program_description("This is a test binary."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    stdio_init_all();  // IMPORTANT

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    sleep_ms(250);
    stdio_flush();  // flush output
    while (stdio_getchar_timeout_us(10)) {}  // clear input buffer

    while (1) {
        char command_buffer[MAX_COMMAND_BUFFER] = "";
        int command_length = 0;

        for (char c = stdio_getchar(); c != '\n' && c != EOF; c = stdio_getchar(), command_length++) {
            command_buffer[command_length] = c;
            printf("%c", c);
            if (command_length >= MAX_COMMAND_BUFFER - 2) break;
        }
        command_buffer[command_length + 1] = '\0';  // terminate input
        
        printf("\nYour command of length %d is: \"%s\"\n", command_length, command_buffer);

        // reset the command buffer and length
        memset(command_buffer, '\0', MAX_COMMAND_BUFFER);  // not needed, but added just in case
        command_length = 0;
    }
}

// usbclient flash --file test.elf --address 5
// load_serial 5
// load_serial 1
// sudo minicom -D /dev/ttyPICO1 115200