#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PASS_1 "first password"
#define PASS_2 "second password"
#define PASS_3 "third password"
#define PASS_4 "fourth password"
#define PASS_5 "fifth password"

#define PRIMARY_PASS "supersec"

#define BUFFER_LEN 17

char strcontains(char* str, char c) {
    while (*str != '\0') if (*str++ == c) return 1;
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