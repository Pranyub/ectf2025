#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "passwords.h"

int main() {

    uint8_t buf[15];

    char* passwords[] = {PASS_1, PASS_2, PASS_3, PASS_4, PASS_5};

    while(1) {
        for (int i = 0; i < 15; i++) {
            buf[i] = getc(stdin);
        }
        // get trailing newline
        getc(stdin);

        uint8_t* passwd = buf + 7;

        if (memcmp(buf, "READ", 4) != 0) {
            continue;
        }

        int index = (int) (buf[5] - '0');

        if (memcmp(passwd, PRIMARY_PASS, 8) != 0) {
            printf("FAIL\n");
            continue;
        }
        printf("reading %d\n", index);
        printf("PASS %s\n", passwords[index]);
    }
    printf("\n");
    return 0;
}