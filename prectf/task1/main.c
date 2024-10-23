#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "passwords.h"

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
