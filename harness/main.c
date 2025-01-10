#include <stdio.h>
#include <string.h>
#include "mxc_device.h"
#include "mxc_delay.h"
#include "nvic_table.h"
#include "pb.h"
#include "board.h"
#include "gpio.h"
#include <stdint.h>
#include "led.h"
#include "inc/keystone.h"
#include <stddef.h>
#include "core_cm4.h"  // CMSIS header for Cortex-M4




const uint DELAY_PIN = 2;
const uint WIDTH_PIN = 3;
const uint ARM_PIN = 4;

#define CMD_MAGIC 0xcc
#define RESP_MAGIC 0xdd
#define CMD_SIZE 10

#define DELAY_PIN_OUT MXC_GPIO_PIN_0
#define DELAY_PORT_OUT MXC_GPIO0

#define ARM_PIN_OUT MXC_GPIO_PIN_1
#define ARM_PORT_OUT MXC_GPIO1


mxc_gpio_cfg_t delay_pin_out;
mxc_gpio_cfg_t arm_pin_out;

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

//Stores value of reg (REGISTERS) in dest (uint32_t*)
#define GET_REGISTER_VALUE(reg, dest)          \
    do {                                        \
        if ((reg) == R0) {                      \
            asm volatile (                      \
                "MOV %0, r0\n"                  \
                : "=r" (*(dest))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R1) {               \
            asm volatile (                      \
                "MOV %0, r1\n"                  \
                : "=r" (*(dest))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R2) {               \
            asm volatile (                      \
                "MOV %0, r2\n"                  \
                : "=r" (*(dest))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R3) {               \
            asm volatile (                      \
                "MOV %0, r3\n"                  \
                : "=r" (*(dest))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R4) {               \
            asm volatile (                      \
                "MOV %0, r4\n"                  \
                : "=r" (*(dest))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R5) {               \
            asm volatile (                      \
                "MOV %0, r5\n"                  \
                : "=r" (*(dest))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R6) {               \
            asm volatile (                      \
                "MOV %0, r6\n"                  \
                : "=r" (*(dest))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R7) {               \
            asm volatile (                      \
                "MOV %0, r7\n"                  \
                : "=r" (*(dest))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R8) {               \
            asm volatile (                      \
                "MOV %0, r8\n"                  \
                : "=r" (*(dest))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R9) {               \
            asm volatile (                      \
                "MOV %0, r9\n"                  \
                : "=r" (*(dest))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == R10) {              \
            asm volatile (                      \
                "MOV %0, r10\n"                 \
                : "=r" (*(dest))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == FP) {               \
            asm volatile (                      \
                "MOV %0, fp\n"                  \
                : "=r" (*(dest))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == IP) {               \
            asm volatile (                      \
                "MOV %0, ip\n"                  \
                : "=r" (*(dest))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == SP) {               \
            asm volatile (                      \
                "MOV %0, sp\n"                  \
                : "=r" (*(dest))               \
                :                               \
                :                               \
            );                                  \
        } else if ((reg) == LR) {               \
            asm volatile (                      \
                "MOV %0, lr\n"                  \
                : "=r" (*(dest))               \
                :                               \
                :                               \
            );                                  \
        }                                       \
    } while (0)


uint8_t exec_memory[128] __attribute__((aligned(2), long_call, section(".data")));


void configureMPU() {
    // Disable MPU
    MPU->CTRL = 0;
}  

void configurePins() {
    delay_pin_out.port = DELAY_PORT_OUT;
    delay_pin_out.mask = DELAY_PIN_OUT;
    delay_pin_out.pad = MXC_GPIO_PAD_NONE;
    delay_pin_out.func = MXC_GPIO_FUNC_OUT;
    delay_pin_out.vssel = MXC_GPIO_VSSEL_VDDIO;
    delay_pin_out.drvstr = MXC_GPIO_DRVSTR_0;
    MXC_GPIO_Config(&delay_pin_out);

    arm_pin_out.port = ARM_PORT_OUT;
    arm_pin_out.mask = ARM_PIN_OUT;
    arm_pin_out.pad = MXC_GPIO_PAD_NONE;
    arm_pin_out.func = MXC_GPIO_FUNC_OUT;
    arm_pin_out.vssel = MXC_GPIO_VSSEL_VDDIO;
    arm_pin_out.drvstr = MXC_GPIO_DRVSTR_0;
    MXC_GPIO_Config(&arm_pin_out);
}


int* runASM(unsigned char* bytecode, int bytecodeSize, int delayUS){
    memcpy(exec_memory, bytecode, bytecodeSize);
    
    void (*exec)(void) = (void (*)(void))((uintptr_t)exec_memory | 1); //Set last bit of function address to 1 so that the blx jump keeps the processor in thumb mode

    //Tell chipwhisper to start listening for delay from GPIO
    MXC_GPIO_OutSet(delay_pin_out.port, delay_pin_out.mask); 
    MXC_Delay(100);
    MXC_GPIO_OutClr(delay_pin_out.port, delay_pin_out.mask); 
    MXC_Delay(1000);

    //Send delay integer through GPIO
    for (int i = 0; i < 32; i++) { //Iterate over all 32 bits of the integer
        int bit = (delayMS >> i) & 1; //Get the i-th bit
        if (bit) {
            MXC_GPIO_OutSet(delay_pin_out.port, delay_pin_out.mask); //Set delay pin to one
        } else {
            MXC_GPIO_OutClr(delay_pin_out.port, delay_pin_out.mask); // Setdelay pin to zero
        }
        MXC_Delay(1000);
    }
    MXC_GPIO_OutClr(delay_pin_out.port, delay_pin_out.mask); //Clear delay pin

    MXC_GPIO_OutSet(arm_pin_out.port, arm_pin_out.mask); //Arm the chipwhisper
    MXC_Delay(100);
    MXC_GPIO_OutClr(arm_pin_out.port, arm_pin_out.mask);
    MXC_Delay(delayMS);  //Delay to sync up with chipwhisperer
    exec(); //Run the bytecode

}   

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

int main(void)
{
    unsigned char opcodes[] = {
    0x03, 0xf1, 0x01, 0x03, 0x03, 0xf1, 0x01, 0x03, 
    0x03, 0xf1, 0x01, 0x03, 0x03, 0xf1, 0x01, 0x03, 
    0x03, 0xf1, 0x01, 0x03, 0x70, 0x47
};

    int count = 0;
    unsigned int cinema = 0;
    configureMPU();
    configurePins();
    while (1) {
        LED_On(LED1);
        MXC_Delay(500000);
        LED_Off(LED1);
        printf("count : %d\n", count++);
        GET_REGISTER_VALUE(R3, &cinema);
        printf("preasm : %d\n", cinema);
        runASM(opcodes, sizeof(opcodes), 0);
        GET_REGISTER_VALUE(R3, &cinema);
        printf("postasm : %d\n", cinema);
        
        MXC_Delay(500000);
    }
}