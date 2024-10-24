#include <stdio.h>
#include <string.h>  // for memset
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "key.h"

#define MAX_COMMAND_BUFFER 35  // string length capped at 34, extra for null
#define MESSAGE_LENGTH 16 // message to be encrypted/decrypted: 16 bytes, or 128 bits 

const uint LED_PIN = 25;

/**
 * @brief Parses a command
 * @param command the command to be parsed.
 * @param password where the extracted password should be stored 
 * @return the password to be fetched, 1-5 inclusive.  Or -1 if there was an
 *              error parsing the command.
 * @pre \p command is 12 characters long and null terminated.
 * @pre \p password is a valid string with a buffer size of 9.
 * @post \p password now stores a null terminated string.
 */
int parse_command(const char* command, char* password);

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
        int command_length = 0;

        // get characters from input and store in buffer
        for (char c = stdio_getchar(); c != '\n' && c != '\r' && c != EOF; c = stdio_getchar()) {
            // check for backspace
            if (c == '\b') {
                printf("\b \b");  // backspace and clear character
                command_length--;
                continue;
            }
            command_buffer[command_length] = c;
            printf("%c", c);
            command_length++;
            if (command_length >= MAX_COMMAND_BUFFER - 1) break;
        }
        puts("");  // newline
        command_buffer[command_length + 1] = '\0';  // terminate input
        
    }
}

// usbclient flash --file test.elf --address 5
// load_serial 5
// sudo minicom -D /dev/ttyPICO5 115200
