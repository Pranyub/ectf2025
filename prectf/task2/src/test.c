#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "example.h"

const uint LED_PIN = 25;

int main() {
        char userInput[15];
    char command[5];
    char index[4];
    char password[9];
    
    while(1)
    {
        printf("READ <1-5> <8 character passcode>\n");

        memset(userInput, 0, strlen(userInput));
        for(int i = 0; i < 15; i++)
        {
            userInput[i] = getc(stdin);
            if(i < 4)
                command[i] = userInput[i];
            else if(i < 7)
                index[i - 4] = userInput[i];
            else
                password[i - 7] = userInput[i];
        }
        getc(stdin);

        if(memcmp(command, "READ", 4) != 0)
        {
            printf("you suck\n");
        }
        
        if(index[1] < 49 | index[1] > 54) {
            printf("invalid index\n");
        }

        if(memcmp(password, PRIMARY_PASS, 8) != 0) {
            printf("incorrect pass\n");
        }
        else {
            printf("Here is password: ");
            if (index[1] = 49) {
                printf(PASS_1);
            } else if (index[1] = 50) {
                printf(PASS_2);
            } else if (index[1] = 51) {
                printf(PASS_3);
            } else if (index[1] = 52) {
                printf(PASS_4);
            } else if (index[1] = 53) {
                printf(PASS_5);
            }
            printf("\n");
        }
    }

    return 0;
}