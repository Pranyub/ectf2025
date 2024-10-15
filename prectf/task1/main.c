#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "password.h"

int main() {
    uint8_t buf[17];
    uint8_t pass[8];

    while (1) {
        fgets(buf, 17, stdin);

        for (int i = 0; i < 17; i++) {
            if (i > 6) {
                pass[i - 7] = buf[i];
            }
        }
        
        if(!strncmp(PRIMARY_PASS, buf + 7, 8)) {
            switch (buf[5]) {
                case '1':
                    printf("PASS " PASS_1 "\n");
                    break;
                case '2':
                    printf("PASS " PASS_2 "\n");
                    break;
                case '3':
                    printf("PASS " PASS_3 "\n");
                    break;
                case '4':
                    printf("PASS " PASS_4 "\n");
                    break;
                case '5':
                    printf("PASS " PASS_5 "\n");
                    break;
                default:
                    printf("FAIL\n");
                    continue;
            }
        };
    }
    return 0;
}