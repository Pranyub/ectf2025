#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "key.h"

#define MAX_COMMAND_BUFFER 35  // string length capped at 34, extra for null
#define MESSAGE_LENGTH 16 // message to be encrypted/decrypted: 16 bytes, or 128 bits 

const uint LED_PIN = 25;

/**
 * @brief Parses a command
 * @param buffer where the command should be stored.
 * @param max_length the maximum length of the command.
 * @param print whether or not to display the characters typed;
 *          non zero is true.
 * @return the length of the command typed.
 * @pre \p buffer is at least of length max_length + 1
 * @pre \p max_length is not less than 0
 * @post \p buffer now stores the command typed up to max_length
 *          plus the null terminator.
 */
int get_command(char* buffer, int max_length);

int main() {
    bi_decl(bi_program_description("This binary is for task2b."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    stdio_init_all();  // IMPORTANT

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    sleep_ms(250);
    stdio_flush();  // flush output
    while (stdio_getchar_timeout_us(10)) {}  // clear input buffer

    while (1) {
        char command_buffer[MAX_COMMAND_BUFFER] = "";
        
        int command_length = get_command(command_buffer, MAX_COMMAND_BUFFER);
        puts("");  // newline
        
        printf("You said: %s\n", command_buffer);
    }
}

int get_command(char* buffer, int max_length) {
    int command_length = 0;
    for (char c = stdio_getchar(); c != '\n' && c != '\r' && c != EOF; c = stdio_getchar()) {
        // check for backspace
        if (c == '\b') {
            printf("\b \b");  // backspace and clear character
            command_length--;
            continue;
        }
        buffer[command_length] = c;
        if (max_length) {
            printf("%c", c);
        }
        command_length++;
        if (command_length >= MAX_COMMAND_BUFFER - 1) break;
    }
    buffer[command_length + 1] = '\0';  // terminate input
    return command_length;
}

// usbclient flash --file test.elf --address 5
// load_serial 5
// sudo minicom -D /dev/ttyPICO5 115200
