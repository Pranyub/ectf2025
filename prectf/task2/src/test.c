#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "aes.h"
#include "example.h"

const uint LED_PIN = 25;

int main() {
    bi_decl(bi_program_description("This is a test binary."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    stdio_init_all(); 

    struct AES_ctx ctx;

    uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    
    uint8_t buf[16];

    AES_init_ctx_iv(&ctx, key, iv);

    while (1) {

        for(int i = 0; i < 16; i++) {
            buf[i] = fgetc(stdin);
        }
        
        printf("\n");

        AES_CBC_encrypt_buffer(&ctx, buf, 16);

        for (int i = 0; i < 16; i++) {
            printf("%02X", buf[i]);
        }

        printf("\n");
    }    
}