#include <stdio.h>
#include "passwords.h"
#include <stdint.h>
#include <string.h>


char* passwords[] = {PASS_1, PASS_2, PASS_3, PASS_4, PASS_5};
// Using a pointer instead of 2D array

uint8_t buf[15];

int main() {

    while(1){
        for(int i=0; i<15; i++){
            buf[i] = getc(stdin);
        }
    

        uint8_t* password = buf + 7;

        if (memcmp(buf, "READ", 4)!=0){
            printf(" READ FAIL \n");
            continue;
        }

        int index = (int)(buf[5]-'0');

        if(memcmp(password, PRIMARY_PASS, 8) != 0){
            printf(" PASSWORD FAIL \n");
            continue;
        }

        printf("PASS %s", passwords[index]);

    }
}