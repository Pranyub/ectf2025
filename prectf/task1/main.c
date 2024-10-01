#include <stdio.h>
#include "passwords.h"
#include <stdint.h>

char passwords[5][16] = {PASS_1, PASS_2, PASS_3, PASS_4, PASS_5}




int main() {

    unit8_t buf[15];

    for (int i=0; i<15; i++){
        buf[i]=getc(stdin);
    }

    if (memcmp(buf, "READ", 4) != 0){
        int passwordIndex = buf[7]
        unit8_t pass[5] = PRIMARY_PASS;
        
        if (memcmp(buf,PRIMARY_PASS,7,15)){
            printf("OKAY OKAY");
        }
    }
}

