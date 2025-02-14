#include "flc.h"
#include "icc.h"
#include "nvic_table.h"

#include <stdio.h>

/**
 * @brief ISR for the Flash Controller
 * 
 * This ISR allows for access to the flash through simple_flash to operate
 */
void flash_irq(void) {
    uint32_t temp;
    temp = MXC_FLC0->intr;

    if (temp & MXC_F_FLC_INTR_DONE) {
        MXC_FLC0->intr &= ~MXC_F_FLC_INTR_DONE;
    }

    if (temp & MXC_F_FLC_INTR_AF) {
        MXC_FLC0->intr &= ~MXC_F_FLC_INTR_AF;
        printf(" -> Interrupt! (Flash access failure)\n\n");
    }
}

void flash_init(void) {
    // Setup Flash
    MXC_NVIC_SetVector(FLC0_IRQn, flash_irq);
    NVIC_EnableIRQ(FLC0_IRQn);
    MXC_FLC_EnableInt(MXC_F_FLC_INTR_DONEIE | MXC_F_FLC_INTR_AFIE);
    MXC_ICC_Disable(MXC_ICC0);
}

int flash_erase_page(uint32_t address) {
    return MXC_FLC_PageErase(address);
}

void flash_read(uint32_t address, void* buffer, uint32_t size) {
    MXC_FLC_Read(address, (uint32_t *)buffer, size);
}

int flash_write(uint32_t address, void* buffer, uint32_t size) {
    return MXC_FLC_Write(address, size, (uint32_t *)buffer);
}
