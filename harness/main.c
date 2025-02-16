#include <stdio.h>
#include <string.h>
#include "inc/main.h"
#include "mxc_device.h"
#include "mxc_delay.h"
#include "nvic_table.h"
#include "pb.h"
#include "board.h"
#include "gpio.h"
#include <stdint.h>
#include "led.h"
#include <stddef.h>
#include "core_cm4.h"  // CMSIS header for Cortex-M4
#include "uart.h"

#define write_ack() write_packet(ACK_MSG, NULL, 0)



int uart_readbyte(void){
    int data = MXC_UART_ReadCharacter(MXC_UART_GET_UART(CONSOLE_UART));
    return data;
}

void uart_flush(void){
    MXC_UART_ClearRXFIFO(MXC_UART_GET_UART(CONSOLE_UART));
}

void uart_writebyte(uint8_t data) {
    while (MXC_UART_GET_UART(CONSOLE_UART)->status & MXC_F_UART_STATUS_TX_FULL) {
    }
    MXC_UART_GET_UART(CONSOLE_UART)->fifo = data;
}

int uart_init(void){
    int ret;

    if ((ret = MXC_UART_Init(MXC_UART_GET_UART(CONSOLE_UART), UART_BAUD, MXC_UART_IBRO_CLK)) != E_NO_ERROR) {
        printf("Error initializing UART: %d\n", ret);
        return ret;
    }

    return E_SUCCESS;
}

int read_bytes(uint8_t buff[], uint16_t len) {
    int result;
    for (int i = 0; i < len; i++) {
        if (i != 0 && i % 256 == 0) { // Send an ACK after receiving 256 bytes
            write_ack();
        }
        result = uart_readbyte();
        if (result < E_SUCCESS) {  // if there was an error, return immediately
            return result;
        }
        buff[i] = result;
    }

    return E_SUCCESS;
}


void read_header(pkt_header* header)
{
    while(uart_readbyte() != MAGIC_BYTE){};
    header->magic = MAGIC_BYTE;
    header->opcode = uart_readbyte() & 0xff;
    uint8_t lengthBytes[] = {0, 0};
    lengthBytes[0] = uart_readbyte() & 0xff;
    lengthBytes[1] = uart_readbyte() & 0xff;
    header->length = (lengthBytes[0] << 8) | (lengthBytes[1] << 0);
}

int read_ack(){ // returns 1 if next packet is an ack, 0 on failure
    pkt_header header;
    read_header(&header);
    return (header.opcode == ACK_MSG ? E_SUCCESS : E_NO_RESPONSE);
}

int write_bytes(uint8_t bytes[], uint16_t len, bool need_ack){
    for (int i = 0; i < len; i++){
        if (i != 0 && i % 256 == 0){
            if (need_ack && read_ack() != E_SUCCESS){
                return E_NO_RESPONSE;
            }
        }
        uart_writebyte(bytes[i]);
    }
    fflush(stdout);
    return E_SUCCESS;
}

int write_packet(opcode_t cmd, uint8_t body[], uint16_t len) {
    pkt_header header;
    header.magic = MAGIC_BYTE;
    header.opcode = cmd;
    header.length = len;

    int result = write_bytes((uint8_t*)(&header), (uint16_t)(len), false);

    if (cmd == ACK_MSG){
        return result;
    }

    if (cmd != DEBUG_MSG && read_ack() != E_SUCCESS) {
        return E_NO_RESPONSE;
    }   

    if (len > 0){
        write_bytes(body, len, cmd != DEBUG_MSG);
        if (cmd != DEBUG_MSG && read_ack() != E_SUCCESS) {
            return E_NO_RESPONSE;
        }   
    }
    return E_SUCCESS;
}

int read_packet(opcode_t* cmd, uint16_t* len, uint8_t body[]) {
    pkt_header header;

    if (cmd == NULL) {
        return E_NULL_PTR;
    }

    read_header(&header);

    *cmd = header.opcode;

    if (len != NULL) {
        *len = (header.length <= MAX_PACKET_LENGTH) ? header.length : MAX_PACKET_LENGTH;
    }

    if (header.opcode != ACK_MSG) {
        write_ack();  
        int result;
        if (header.length && body != NULL) {
            result = read_bytes(body, header.length);
            if (result != E_SUCCESS) {
                return result;
            }
        }
        if (header.length) {
            result = write_ack();
            if (result != E_SUCCESS) { 
                return result;
            }
        }
    }
    return E_SUCCESS;
}
int main(void)
{
    if (uart_init()!= E_NO_ERROR){
        return;
    }
    uint8_t buff[MAX_PACKET_LENGTH];
    int result;
    opcode_t cmd;
    uint16_t pkt_len;
    uint8_t test[3] = {'a', 'b', 'c'};
    while (1) {
        LED_On(LED1);
        result = read_packet(&cmd, &pkt_len, buff);
        if (result == E_SUCCESS){
           switch (cmd) {
                case DECODE_MSG:
                    write_packet(DECODE_MSG, test, sizeof(test));
                    break;
                case SUBSCRIBE_MSG:
                    write_packet(SUBSCRIBE_MSG, test, sizeof(test));
                    break;
                case LIST_MSG:
                    write_packet(LIST_MSG, test, sizeof(test));
                    break;
                default:
                    break;
            }
        }
        else{
            uart_writebyte(result);
        }
        uart_flush();
    }
}
