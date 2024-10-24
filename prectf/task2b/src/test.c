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
 * @brief Stores a command into a buffer
 * @param buffer where the command should be stored.
 * @param max_length the maximum length of the command.
 * @param print whether or not to display the characters typed;
 *          non zero is true.
 * @return the length of the command typed.
 * @pre \p buffer is at least of length \p max_length + 1
 * @pre \p max_length is not less than 0
 * @post \p buffer now stores the command typed up to max_length characters
 *          plus the null terminator.
 */
int get_command(char* buffer, int max_length);

/**
 * @brief Parses a command and returns 
 * @param command the command that should be parsed.
 * @param message pointer to where the message should be stored.
 * @return 0 on success, or -1 if there was an error parsing the command.
 * @pre \p command is of length MAX_COMMAND_BUFFER and null terminated.
 * @pre \p message is of length MAX_MESSAGE_LENGTH.
 * @post \p message now stores the message encoded as hex.
 */
int parse_command(const char* command, unsigned char* message);

/**
 * @brief converts an ASCII character representing a hex character to decimal.
 * @param character the ASCII character to be converted.
 * @return the decimal representation of \p character or -1 on error.
 * @pre \p character is an ASCII character from 0-9 or a-f (MUST be lowercase).
 */
int char_to_decimal(char character);

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
        
        unsigned char message[MESSAGE_LENGTH];
        if (parse_command(command_buffer, message) != 0) {
            puts("Error parsing the command.");
            continue;
        }
        // message now contains the message as a series of hex numbers
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

int parse_command(const char* command, unsigned char* message) {
    // check formatting
    if (strnlen(command, MAX_COMMAND_BUFFER) != MAX_COMMAND_BUFFER - 1 ||
        command[0] != 'S' ||
        command[1] != ' ') return -1;
    const int START_INDEX = 2;  // the index where the message starts

    // loop through first character for each message
    for (int i = START_INDEX; i < MAX_COMMAND_BUFFER - 1; i += 2) {
        // convert char to decimal
        int sixteenths_place = 0;
        int ones_place = 0;
        if ((sixteenths_place = char_to_decimal(command[i])) == -1 ||
            (ones_place = char_to_decimal(command[i+1])) == -1) return -1;
        message[(i - START_INDEX) / 2] = (char)(sixteenths_place * 16 + ones_place);
    }
    puts("");
    return 0;
}

int char_to_decimal(char character) {
    // 0 - 9
    if (character - '0' >= 0 && character - '9' <= 0) return character - '0';
    // a-f
    if (character - 'a' >= 0 && character - 'f' <= 0) return character - 'a' + 10;
    return -1;  // not a hex character
}

// usbclient flash --file test.elf --address 5
// load_serial 5
// sudo minicom -D /dev/ttyPICO5 115200
