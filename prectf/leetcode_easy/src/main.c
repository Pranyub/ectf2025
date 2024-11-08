#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

const uint LED_PIN = 25;

// 1 true, 0 false
int is_palindrome(int number);

int main() {
    bi_decl(bi_program_description("This binary is for a leetcode challenge."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    stdio_init_all();  // IMPORTANT

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    sleep_ms(250);
    stdio_flush();  // flush output

    while(1) {
        char numbers[10] = ""; 
        int length = 0;
        printf("Enter a number to check if it is a palindrome (less than 10 digits please): ");
        for (char c = stdio_getchar(); c != '\n' && c != '\r' && c != EOF; c = stdio_getchar()) {  // getting characters
            numbers[length] = c;
            length++;
        }
    }
    return 0;
}

int is_palindrome(int number) {
    if (number < 0) return 0;
    int length = (int)log10(number) + 1;
    while (number > 0) {
        if ((number % 10) != (number / (int)pow(10, length - 1))) return 0;
        number -= pow(10, length - 1);
        number /= 10;
        length -= 2;
    }
    return 1;
}

// https://leetcode.com/problems/palindrome-number/