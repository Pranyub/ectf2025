#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "example.h"
#include <ctype.h>
#include <string.h>

#include "aes.h"

#define PASS_1 "first password"
#define PASS_2 "second password"
#define PASS_3 "third password"
#define PASS_4 "fourth password"
#define PASS_5 "fifth password"
#define PRIMARY_PASS "supersec"

const uint LED_PIN = 25;

void task_2a(void) {
    static char* passwords[] = {PASS_1, PASS_2, PASS_3, PASS_4, PASS_5};
    while (1) {
        char buf[13];
        fgets(buf, sizeof(buf), stdin);
        if (buf[0] == 'R') {
            char num = buf[2] - '0';
            if (num >= 1 && num <= 5) {
                char* passcode = &buf[4];
                if (!strncmp(PRIMARY_PASS, passcode, strlen(PRIMARY_PASS))) {
                    printf("P %s", passwords[num - 1]);
                    continue;
                }
            }
        }
        puts("FAIL");
    }
}

void task_2b(void) {
    while (1) {
        char buf[32];
        if (scanf("0x%32s", buf) != 1) {
            puts("FAIL");
            continue;
        }

        // TODO: use scanf instead
        unsigned char input[16];
        for (int i = 0; i < sizeof(buf); i+=2) {
            if (isdigit(buf[i]) && isdigit(buf[i+1])) {
                input[i/2] = ((buf[i]-'0') < 4) | (buf[i+1]-'0');
            }
        }

        struct AES_ctx ctx = {0};
        unsigned char key[] = { 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef};
        AES_init_ctx(&ctx, key);
        AES_ECB_encrypt(&ctx, input);

        printf("R ");
        for (int i = 0; i < sizeof(input); i++) {
            printf("%02x", input[i]);
        }
    }
}

int main() {
    bi_decl(bi_program_description("Task 2 Solution"));
    stdio_init_all();
    task_2b();
}