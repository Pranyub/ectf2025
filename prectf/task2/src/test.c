#include <stdio.h>
// #include <unistd.h>
#include <string.h>  // for memset
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
// #include "pico/pico_mem_ops.h"  // for pico memset
// #include "example.h"  // for testing
#include "passwords.h"

#define MAX_COMMAND_BUFFER 13  // string length capped at 12, extra for null
#define PASSWORD_LENGTH 8  // primary pass length

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

        // get characters from input and store in buffer
        for (char c = stdio_getchar(); c != '\n' && c != '\r' && c != EOF; c = stdio_getchar()) {
            // check for backspace
            if (c == '\b') {
                printf("\b");
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
        

        char password[PASSWORD_LENGTH + 1] = "";
        int index = parse_command(command_buffer, password);
        if (command_length != 12 ||
            strncmp(password, PRIMARY_PASS, PASSWORD_LENGTH) != 0) {
            index = -1;  // causes the default case in the switch
        }

        switch (index) {
            case 1:
                printf("P %s\n", PASS_1);
                break;
            case 2:
                printf("P %s\n", PASS_2);
                break;
            case 3:
                printf("P %s\n", PASS_3);
                break;
            case 4:
                printf("P %s\n", PASS_4);
                break;
            case 5:
                printf("P %s\n", PASS_5);
                break;
            default:
                puts("FAIL");
        }
    }
}

int parse_command(const char* command, char* password) {
    if (strnlen(command, MAX_COMMAND_BUFFER) != 12 ||
        command[0] != 'R' ||
        command[1] != ' ' ||
        command[3] != ' ') return -1;

    int index = command[2] - '0';
    if (index < 1 || index > 5) return -1;
    strncpy(password, &(command[4]), PASSWORD_LENGTH);
    return index;
}

// usbclient flash --file test.elf --address 5
// load_serial 5
// sudo minicom -D /dev/ttyPICO5 115200
