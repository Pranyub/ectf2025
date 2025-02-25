/**
 * @file "simple_uart.c"
 * @author Samuel Meyers
 * @brief UART Interrupt Handler Implementation 
 * @date 2025
 *
 * This source file is part of an example system for MITRE's 2025 Embedded System CTF (eCTF).
 * This code is being provided only for educational purposes for the 2025 MITRE eCTF competition,
 * and may not meet MITRE standards for quality. Use this code at your own risk!
 *
 * @copyright Copyright (c) 2025 The MITRE Corporation
 */

#include "simple_uart.h"

/** @brief Initializes the UART Interrupt handler.
 * 
 *  @note This function should be called once upon startup.
 *  @return 0 upon success.  Negative if error.
*/
int uart_init(void){
    #ifdef LINUX_BUILD
    return 0;
    #else
    int ret;

    if ((ret = MXC_UART_Init(MXC_UART_GET_UART(CONSOLE_UART), UART_BAUD, MXC_UART_IBRO_CLK)) != E_NO_ERROR) {
        printf("Error initializing UART: %d\n", ret);
        return ret;
    }

    return E_NO_ERROR;
    #endif
}

/** @brief Reads a byte from UART and reports an error if the read fails.
 * 
 *  @return The character read.  Otherwise see MAX78000 Error Codes for
 *      a list of return codes.
*/
int uart_readbyte_raw(void){
    #ifdef LINUX_BUILD
    int data = getchar();
    #else
    int data = MXC_UART_ReadCharacterRaw(MXC_UART_GET_UART(CONSOLE_UART));
    #endif
    return data;
}

/** @brief Reads the next available character from UART.
 * 
 *  @return The character read.  Otherwise see MAX78000 Error Codes for
 *      a list of return codes.
*/
int uart_readbyte(void){
    #ifdef LINUX_BUILD
    int data = getchar();
    #else
    int data = MXC_UART_ReadCharacter(MXC_UART_GET_UART(CONSOLE_UART));
    #endif
    return data;
}

/** @brief Writes a byte to UART.
 * 
 *  @param data The byte to be written.
*/
void uart_writebyte(uint8_t data) {
    #ifdef LINUX_BUILD
    putchar(data);
    #else
    while (MXC_UART_GET_UART(CONSOLE_UART)->status & MXC_F_UART_STATUS_TX_FULL) {
    }
    MXC_UART_GET_UART(CONSOLE_UART)->fifo = data;
    #endif
}

/** @brief Flushes UART.
*/
void uart_flush(void){
    #ifdef LINUX_BUILD
    // flush stdin
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    // flush stdout
    fflush(stdout);
    #else
    MXC_UART_ClearRXFIFO(MXC_UART_GET_UART(CONSOLE_UART));
    MXC_UART_ClearTXFIFO(MXC_UART_GET_UART(CONSOLE_UART));
    #endif
}
