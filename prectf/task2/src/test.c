#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "pico/unique_id.h"
#include "example.h"

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
        for (int len = 0; len < 8; len++) {
            printf("%02X", id.id[len]);
        }
        char c = getchar();
        printf("You said: %c\n", c);
    }
}