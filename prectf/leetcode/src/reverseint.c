#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/unique_id.h"

const uint LED_PIN = 25;

int main() {
    bi_decl(bi_program_description("This is a test binary."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    stdio_init_all(); // DO NOT REMOVE. All user code should be after this

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    pico_unique_board_id_t id;
    pico_get_unique_board_id(&id);
    while (1) {
        __uint64_t num = 0;
        int valid = 0;
        
        printf("Enter a number: ");
        while (!valid) {
            int c = getchar(); // Read a single character from input
            
            if (c >= '0' && c <= '9') {
                // If the character is a digit, add it to the number
                num = num * 10 + (c - '0');
            } else if (c == '\r' || c == '\n') {
                // If a newline or carriage return is received, end input
                valid = 1;
            } else {
                // If an invalid character is encountered, reset and prompt again
                printf("\nInvalid input. Please enter a valid number: ");
                num = 0;  // Reset the number
            }
        }
                
        __uint64_t reversedNum = 0;
        __uint64_t counter = 1;

        printf("\nNon-reversed Number: %llu\n", num);
        
        while (num > 0) {
            reversedNum *= 10;
            reversedNum += (num % 10);
            num = num / 10;
        }

        printf("Reversed Number: %lld\n", reversedNum);
    }
}