#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

int main(){    
    bi_decl(bi_program_description("This is a test binary."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    stdio_init_all(); 

    unsigned int r;

    FILE *frand=fopen("/dev/random","r");

    while (1) {
        fread(&r,sizeof(unsigned int),1,frand);
        printf("Your random number is %u.\n",r);
        sleep(1);
    }
}