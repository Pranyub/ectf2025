#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "password.h"

#define MAX_COMMAND_BUFFER 16
int main() {
    char command_buffer[MAX_COMMAND_BUFFER] = "";
    while (fgets(command_buffer, MAX_COMMAND_BUFFER, stdin) != NULL) {
        if (strnlen(command_buffer, MAX_COMMAND_BUFFER) != 15) {
            puts("FAIL");
            return 1;
        }

        char* token = strtok(command_buffer, " ");

        if (token != NULL && strncmp(token, "PASS", MAX_COMMAND_BUFFER) != 0) {
            puts("FAIL");
            return 1;
        }

        token = strtok(NULL, " ");
        int password = 0;

        if (token == NULL) {
            puts("FAIL");
            return 1;
        }

        password = token[0] - '0';

        if (password < 1 || password > 5) {
            puts("FAIL");
            return 1;
        }

        token = strtok(NULL, " ");
        token[strnlen(token, MAX_COMMAND_BUFFER)] = '\0';
        if (token == NULL || strncmp(token, PRIMARY_PASS, MAX_COMMAND_BUFFER) != 0) {
            puts("FAIL");
            return 1;
        }

        printf("PASS ");
        switch (password) {
            case 1:
                puts(PASS_1);
                break;
            case 2:
                puts(PASS_2);
                break;
            case 3:
                puts(PASS_3);
                break;
            case 4:
                puts(PASS_4);
                break;
            case 5:
                puts(PASS_5);
                break;
            default: break;
        }
        getchar();
    }
    
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