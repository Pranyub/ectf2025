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
#include "uart.h"


#define UART_BAUD 115200
#define BUFF_SIZE 1024
#define READING_UART MXC_UART2
#define WRITING_UART MXC_UART3
volatile int READ_FLAG;

    #define CMD_MAGIC 0xcc
    #define RESP_MAGIC 0xdd
    #define CMD_SIZE 512

    #define DELAY_PIN_OUT MXC_GPIO_PIN_0
    #define DELAY_PORT_OUT MXC_GPIO0

    #define ARM_PIN_OUT MXC_GPIO_PIN_1
    #define ARM_PORT_OUT MXC_GPIO1


    mxc_gpio_cfg_t delay_pin_out;
    mxc_gpio_cfg_t arm_pin_out;

    enum CMD_ID {
        READ_MEM    = 0x01,
        WRITE_MEM   = 0x02,
        READ_REG    = 0x03,
        RUN_ASM     = 0x04,
        HELLO_WORLD = 0x05,
        SET_CW_DELAY = 0x06,
        SET_CW_LENGTH = 0x07,
        SEND_REG = 0x08,
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

// void readCallback(mxc_uart_req_t *req, int error)
// {
//     READ_FLAG = error;
// }

// void configureUART() {
//     uint8_t TxData[BUFF_SIZE];
//     uint8_t RxData[BUFF_SIZE];
//     for (int i = 0; i < BUFF_SIZE; i++) {
//         TxData[i] = i;
//     }

//     memset(RxData, 0x0, BUFF_SIZE);

//     MXC_UART_Init(READING_UART, UART_BAUD, MXC_UART_APB_CLK);
//     MXC_UART_Init(WRITING_UART, UART_BAUD, MXC_UART_APB_CLK);

//     mxc_uart_req_t read_req;
//     read_req.uart = READING_UART;
//     read_req.rxData = RxData;
//     read_req.rxLen = BUFF_SIZE;
//     read_req.txLen = 0;
//     read_req.callback = readCallback;

//     mxc_uart_req_t write_req;
//     write_req.uart = WRITING_UART;
//     write_req.txData = TxData;
//     write_req.txLen = BUFF_SIZE;
//     write_req.rxLen = 0;
//     write_req.callback = NULL;

//     READ_FLAG = 1;
//     MXC_UART_ClearRXFIFO(READING_UART);
// }

uint32_t runLengthTestReg(unsigned char* bytecode, int bytecodeSize, REGISTERS reg, int lengthUS, int delayUS){
    memcpy(exec_memory, bytecode, bytecodeSize);
    
    void (*exec)(void) = (void (*)(void))((uintptr_t)exec_memory | 1); //Set last bit of function address to 1 so that the blx jump keeps the processor in thumb mode

    // //Tell chipwhisper to start listening for delay from GPIO
    // MXC_GPIO_OutSet(delay_pin_out.port, delay_pin_out.mask); 
    // MXC_Delay(100);
    // MXC_GPIO_OutClr(delay_pin_out.port, delay_pin_out.mask); 
    // MXC_Delay(1000);

    // //Send delay integer through GPIO
    // for (int i = 0; i < 32; i++) { //Iterate over all 32 bits of the integer
    //     int bit = (delayMS >> i) & 1; //Get the i-th bit
    //     if (bit) {
    //         MXC_GPIO_OutSet(delay_pin_out.port, delay_pin_out.mask); //Set delay pin to one
    //     } else {
    //         MXC_GPIO_OutClr(delay_pin_out.port, delay_pin_out.mask); // Setdelay pin to zero
    //     }
    //     MXC_Delay(1000);
    // }
    // MXC_GPIO_OutClr(delay_pin_out.port, delay_pin_out.mask); //Clear delay pin

    // MXC_GPIO_OutSet(arm_pin_out.port, arm_pin_out.mask); //Arm the chipwhisper
    // MXC_Delay(100);
    // MXC_GPIO_OutClr(arm_pin_out.port, arm_pin_out.mask);

    
    //Send delay and length someitme

    //MXC_Delay(delayUS);  //Delay to sync up with chipwhisperer
    exec(); //Run the bytecode
    uint32_t value;
    GET_REGISTER_VALUE(reg, &value);
    return value;
}   

void send(char bytes[], int length ){
    for (int i = 0; i < length; i ++){
        putchar(bytes[i]);
    }
}

int repl() {
    uint8_t cmd[CMD_SIZE];

    uint8_t c;
   
    // Wait for magic
    // while ((c = getchar()) != CMD_MAGIC) {};
    
    char k[] = {CMD_MAGIC, HELLO_WORLD};
    send(k, sizeof(k));
    return;
    // First char is magic
    cmd[0] = c;
    
    // Next char is command ID

    cmd[1] = getchar();

    char command = cmd[1];
    if (command = HELLO_WORLD){
        char resp[] = {CMD_MAGIC, HELLO_WORLD};
        send(resp, sizeof(resp));
        return 0;
    }
    
    if (command == RUN_ASM){
        char a[] = {CMD_MAGIC, HELLO_WORLD};
        send(a, sizeof(a));
        return;
        //Magic, CMD_ID, Reg Or Mem, Register or Address (4 bytes), Opcodes (128 bytes), target state opcodes (128 bytes)
        //Command Size is 263 bytes
        for(int i = 2; i < 263; i++) {
            cmd[i] = getchar();
        }
        char readType = cmd[2];
        unsigned char testingOpcodes[128];
        unsigned char targetStateOpcodes[128];
        //Read from register
        if (readType == 0x00){
            char reg = cmd[3];
            for (int i = 7; i < 128 + 7; i++){
                testingOpcodes[i - 7] = cmd[i];
            }
            for (int i = 128 + 7; i < 263; i++){
                targetStateOpcodes[i - 128 - 7] = cmd[i];
            }
            uint32_t targetRegisterValue = runLengthTestReg(targetStateOpcodes, 128, reg, 0, 0);
            uint8_t resp1[7] = {CMD_MAGIC, SEND_REG, reg};
            resp1[3] = (targetRegisterValue >> 24) & 0xff;
            resp1[4] = (targetRegisterValue >> 16) & 0xff;
            resp1[5] = (targetRegisterValue >> 8 ) & 0xff;
            resp1[6] = (targetRegisterValue >> 0 ) & 0xff;
            char a[] = {CMD_MAGIC, HELLO_WORLD};
            send(a, sizeof(a));
            uint32_t testRegisterValue = 0;
            int delay = 1000;
            int length = 1000;
            do{
                testRegisterValue = runLengthTestReg(testingOpcodes, 128, reg, delay, length);
                uint8_t resp[7] = {CMD_MAGIC, SEND_REG, reg};
                resp[3] = (testRegisterValue >> 24) & 0xff;
                resp[4] = (testRegisterValue >> 16) & 0xff;
                resp[5] = (testRegisterValue >> 8 ) & 0xff;
                resp[6] = (testRegisterValue >> 0 ) & 0xff;
                send(a, sizeof(a));   
                length += 100;
            }while(0);
        }       
        return 0;
    }
}
    
    // Read in rest of command
    
    
    // // Parse data for command (big endian)
    // uint32_t addr = (cmd[2] << 24) | (cmd[3] << 16) | (cmd[4] << 8) | (cmd[5] << 0);
    // uint32_t write_data = (cmd[6] << 24) | (cmd[7] << 16) | (cmd[8] << 8) | (cmd[9] << 0);

    // uint8_t resp[CMD_SIZE] = {0};

    // // first byte is magic, second byte is command type
    // resp[0] = RESP_MAGIC;
    // resp[1] = command;
    // if (command == READ_MEM) {
    //     uint32_t data = *(uint32_t*)(addr);
    //     resp[2] = (data >> 24) & 0xff;
    //     resp[3] = (data >> 16) & 0xff;
    //     resp[4] = (data >> 8 ) & 0xff;
    //     resp[5] = (data >> 0 ) & 0xff;
    // } else if (command == WRITE_MEM) {
    //     *(uint32_t*)(addr) = write_data;
    // }
    // else if (command == READ_REG)
    // {
    //     uint32_t reg;
    //     GET_REGISTER_VALUE(cmd[2], &reg);
    // }

    // // for(int i = 0; i < CMD_SIZE; i++) {
    // //     putchar(resp[i]);
    // // }

//     return 0;
// }


void pissballs(unsigned char cmd[]){
        char readType = cmd[2];
        unsigned char testingOpcodes[128];
        unsigned char targetStateOpcodes[128];
        char a[] = {CMD_MAGIC, HELLO_WORLD, 0x01};
        send(a, sizeof(a));
        //Read from register
        if (readType == 0x00){
            char reg = cmd[3];
            for (int i = 7; i < 128 + 7; i++){
                testingOpcodes[i - 7] = cmd[i];
            }
            for (int i = 128 + 7; i < 263; i++){
                targetStateOpcodes[i - 128 - 7] = cmd[i];
            }
             a[2] = 0x02;
            send(a, sizeof(a));
            uint32_t targetRegisterValue = runLengthTestReg(targetStateOpcodes, 128, reg, 0, 0);
            uint8_t resp1[7] = {CMD_MAGIC, SEND_REG, reg};
            resp1[3] = (targetRegisterValue >> 24) & 0xff;
            resp1[4] = (targetRegisterValue >> 16) & 0xff;
            resp1[5] = (targetRegisterValue >> 8 ) & 0xff;
            resp1[6] = (targetRegisterValue >> 0 ) & 0xff;
            send(resp1, sizeof(resp1));
            uint32_t testRegisterValue = 0;
            int delay = 1000;
            int length = 1000;
            do{
                testRegisterValue = runLengthTestReg(testingOpcodes, 128, reg, delay, length);
                uint8_t resp[7] = {CMD_MAGIC, SEND_REG, reg};
                resp[3] = (testRegisterValue >> 24) & 0xff;
                resp[4] = (testRegisterValue >> 16) & 0xff;
                resp[5] = (testRegisterValue >> 8 ) & 0xff;
                resp[6] = (testRegisterValue >> 0 ) & 0xff;
                send(resp, sizeof(resp));   
                length += 100;
            }while(0);
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

unsigned char cmd[] = {
    // Initial cmd values
    CMD_MAGIC, RUN_ASM, 0x00, R3, 0x00, 0x00, 0x00,

    // opcodesTest with padding
    0x03, 0xf1, 0x01, 0x03, 0x03, 0xf1, 0x01, 0x03, 
    0x03, 0xf1, 0x01, 0x03, 0x03, 0xf1, 0x01, 0x03, 
    0x03, 0xf1, 0x01, 0x03, 0x70, 0x47, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00,

    // opcodesTarget with padding
    0x03, 0xf1, 0x01, 0x03, 0x03, 0xf1, 0x01, 0x03, 
    0x03, 0xf1, 0x01, 0x03, 0x70, 0x47, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00
};

    int count = 0;
    unsigned int cinema = 0;
    configureMPU();
    while (1) {
        LED_On(LED1);
        // repl();
        // MXC_Delay(500000);
        pissballs(cmd);
        LED_Off(LED1);
        MXC_Delay(5000);
        // while (getchar() != '\n'); //Clear input buffer

    }
}