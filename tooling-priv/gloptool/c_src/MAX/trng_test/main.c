/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. (now owned by 
 * Analog Devices, Inc.),
 * Copyright (C) 2023-2024 Analog Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

/**
 * @file    main.c
 * @brief   Random number
 * @details This example uses the UART to print a random number.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include "mxc_device.h"
#include "trng.h"

/***** Definitions *****/

/***** Globals *****/
uint8_t var_rnd_no[16] = { 0 };

/***** Functions *****/

void print(char *stuff)
{
    int i, j, size = 4;

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            printf("0x%02x ", stuff[i * size + j]);
        }

        printf("\n");
    }

    return;
}

int main(void)
{
    int num_bytes = 16;
    puts("Random number 16 byte number test\n");
    MXC_TRNG_Init();
    MXC_TRNG_Random(var_rnd_no, num_bytes);
    print((char*)var_rnd_no);
    while (1) {}
}
