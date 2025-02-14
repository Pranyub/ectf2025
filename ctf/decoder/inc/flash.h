#pragma once

#include <stdint.h>

/**
 * @brief Initialize the Simple Flash Interface
 * 
 * This function registers the interrupt for the flash system,
 * enables the interrupt, and disables ICC
*/
void flash_init(void);

/**
 * Erases a page from flash
*/
int flash_erase_page(uint32_t address);

/**
 * Reads a page out of the flash memory
*/
void flash_read(uint32_t address, void* buffer, uint32_t size);

/**
 * Writes a page into flash memory, the page must first be erased
 */
int flash_write(uint32_t address, void* buffer, uint32_t size);