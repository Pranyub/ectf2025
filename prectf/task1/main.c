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

int main() {
    char buffer[BUFFER_LEN];
    while (1) {
        // get input
        if (fgets(buffer, BUFFER_LEN, stdin) == NULL) exit(0);

        // consume up until newline
        if (!strcontains(buffer, '\n')) {
            char secbuf[BUFFER_LEN];
            do {
                if (fgets(secbuf, BUFFER_LEN, stdin) == NULL) exit(1);
            } while (!strcontains(secbuf, '\n'));
            printf("FAIL\n");
            continue;
        }


        // validate
        if (buffer[6] != ' ' || buffer[15] != '\n' || strncmp("READ ", buffer, 5) || strncmp(PRIMARY_PASS, buffer+7, 8)) {
            printf("FAIL\n");
            continue;
        }
        // fetch password
        switch (buffer[5]) {
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
    }
}
