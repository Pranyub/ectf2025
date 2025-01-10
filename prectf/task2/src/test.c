#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "example.h"
#include "keystone.h"


const uint DELAY_PIN = 2;
const uint WIDTH_PIN = 3;
const uint ARM_PIN = 4;

#define CMD_MAGIC 0xcc
#define RESP_MAGIC 0xdd
#define CMD_SIZE 10

enum CMD_ID {
    READ_MEM    = 0x01,
    WRITE_MEM   = 0x02,
    READ_REG    = 0x03
};

typedef enum{
    R0,
    R1,
    R2, 
    R3,
    R4,
    R5, 
    R6,
    R7, 
    R8,
    R9,
    R10,
    FP,
    IP,
    SP, 
    LR,
    PC
} REGISTERS;

//reg is REGISTERS
//value is uint32_t*
#define GET_REGISTER_VALUE(reg, value)          \
    do {                                        \
        if ((reg) == R0) {                      \
            asm volatile (                      \
                "MOV %0, r0\n"                  \
                : "=r" (*(value))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R1) {               \
            asm volatile (                      \
                "MOV %0, r1\n"                  \
                : "=r" (*(value))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R2) {               \
            asm volatile (                      \
                "MOV %0, r2\n"                  \
                : "=r" (*(value))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R3) {               \
            asm volatile (                      \
                "MOV %0, r3\n"                  \
                : "=r" (*(value))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R4) {               \
            asm volatile (                      \
                "MOV %0, r4\n"                  \
                : "=r" (*(value))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R5) {               \
            asm volatile (                      \
                "MOV %0, r5\n"                  \
                : "=r" (*(value))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R6) {               \
            asm volatile (                      \
                "MOV %0, r6\n"                  \
                : "=r" (*(value))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R7) {               \
            asm volatile (                      \
                "MOV %0, r7\n"                  \
                : "=r" (*(value))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R8) {               \
            asm volatile (                      \
                "MOV %0, r8\n"                  \
                : "=r" (*(value))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R9) {               \
            asm volatile (                      \
                "MOV %0, r9\n"                  \
                : "=r" (*(value))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R10) {              \
            asm volatile (                      \
                "MOV %0, r10\n"                 \
                : "=r" (*(value))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == FP) {               \
            asm volatile (                      \
                "MOV %0, fp\n"                  \
                : "=r" (*(value))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == IP) {               \
            asm volatile (                      \
                "MOV %0, ip\n"                  \
                : "=r" (*(value))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == SP) {               \
            asm volatile (                      \
                "MOV %0, sp\n"                  \
                : "=r" (*(value))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == LR) {               \
            asm volatile (                      \
                "MOV %0, lr\n"                  \
                : "=r" (*(value))               \
                :                               \
                :                               \
            );                                  \
        }                                       \
    } while (0)


unsigned char* encodeASM(const char* assembly){
    ks_engine *ks;
    ks_err err;
    size_t count;
    unsigned char *encode;
    size_t size;

    err = ks_open(KS_ARCH_ARM, KS_MODE_ARM, &ks);
    if (err != KS_ERR_OK) {
        printf("ERROR: failed on ks_open(), quit\n");
        return;
    }

    if (ks_asm(ks, assembly, 0, &encode, &size, &count)) {
        printf("ERROR: failed on ks_asm() with count = %lu, error code = %u\n", count, ks_errno(ks));
    } else {
        size_t i;

        printf("%s = ", assembly);
        for (i = 0; i < size; i++) {
            printf("%02x ", encode[i]);
        }
        printf("\n");
        printf("Assembled: %lu bytes, %lu statements\n\n", size, count);
    }
    size_t encodeSize = sizeof(encode);
    unsigned char * returnedBytes = (unsigned char *)malloc(encodeSize);
    memcpy(returnedBytes, encode, size);
    // NOTE: free encode after usage to avoid leaking memory
    ks_free(encode);

    // close Keystone instance when done
    ks_close(ks);
    return returnedBytes;
}

void runASM(unsigned char* bytecode, int delayMS){
    size_t size = 1024;
    void *buffer = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    memcpy(buffer, bytecode, sizeof(bytecode));
    void (*func)() = (void (*)())buffer;
    gpio_put(DELAY_PIN, delayMS);
    gpio_put(ARM_PIN, 1);
    sleep_ms(delayMS);
}   int result = func();

int repl() {
    uint8_t cmd[CMD_SIZE];

    uint8_t c;
    
    // Wait for magic
    while((c = getchar()) != CMD_MAGIC);
    
    // First char is magic
    cmd[0] = c;
    // Read in rest of command
    for(int i = 1; i < CMD_SIZE; i++) {
        cmd[i] = getchar();
    }

    // Command Id
    char command = cmd[1];
    // Parse data for command (big endian)
    uint32_t addr = (cmd[2] << 24) | (cmd[3] << 16) | (cmd[4] << 8) | (cmd[5] << 0);
    uint32_t write_data = (cmd[6] << 24) | (cmd[7] << 16) | (cmd[8] << 8) | (cmd[9] << 0);

    uint8_t resp[CMD_SIZE] = {0};

    // first byte is magic, second byte is command type
    resp[0] = RESP_MAGIC;
    resp[1] = command;
    if (command == READ_MEM) {
        uint32_t data = *(uint32_t*)(addr);
        resp[2] = (data >> 24) & 0xff;
        resp[3] = (data >> 16) & 0xff;
        resp[4] = (data >> 8 ) & 0xff;
        resp[5] = (data >> 0 ) & 0xff;
    } else if (command == WRITE_MEM) {
        *(uint32_t*)(addr) = write_data;
    }
    else if (command == READ_REG)
    {
        uint32_t reg;
        GET_REGISTER_VALUE(cmd[2], &reg);
    }

    for(int i = 0; i < CMD_SIZE; i++) {
        putchar(resp[i]);
    }

    return 0;
}

int main() {
    bi_decl(bi_program_description("This is a test binary."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));
    stdio_init_all(); // DO NOT REMOVE. All user code should be after this
    
    gpio_init(DELAY_PIN);
    gpio_set_dir(DELAY_PIN, GPIO_OUT);

    gpio_init(WIDTH_PIN);
    gpio_set_dir(WIDTH_PIN, GPIO_OUT);
    
    uint32_t val = 0;
    while(1) {
        puts("starting");
        GET_REGISTER_VALUE(R5, &val);
        printf("val before asm: %d\n", val);
        runASM("add r5, r5, #0xf");
        GET_REGISTER_VALUE(R5, &val);
        printf("val aftr asm: %d\n", val);
        sleep_ms(100);
    }
}

int piss(){

    return 0;
}