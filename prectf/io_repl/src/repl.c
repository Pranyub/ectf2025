#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"
#include "example.h"

const uint LED_PIN = 25;

#define CMD_MAGIC 0xcc
#define RESP_MAGIC 0xdd
#define CMD_SIZE 10

enum CMD_ID {
    READ_MEM    = 0x01,
    WRITE_MEM   = 0x02
};

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

    for(int i = 0; i < CMD_SIZE; i++) {
        putchar(resp[i]);
    }

    return 0;
}

int main() {
    bi_decl(bi_program_description("This is a test binary."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    stdio_init_all(); // DO NOT REMOVE. All user code should be after this
    
    while(1) {
        repl();
    }
}