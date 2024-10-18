#include <stdio.h>
#include <stdint.h>

int main(){
    uint8_t buf[5];

    printf("WRITE THE WORD: ");
    for(int i; i<5; i++){
        buf[i] = getc(stdin);
    }

    uint8_t* reverse = buf - 1;

    printf("%d", (int)(reverse-'0'));

}

