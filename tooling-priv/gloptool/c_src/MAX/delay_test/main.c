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
 * @brief   Delay test
 * @details This example uses the UART to print to a message every 5 seconds.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include "mxc_device.h"
#include "mxc_delay.h"

/***** Definitions *****/

/***** Globals *****/

/***** Functions *****/

// *****************************************************************************
int main(void)
{
    printf("Delay test.\n");
    printf("5s\n");
    MXC_Delay(5000000);  // SysTick
    printf("1s\n");
    MXC_Delay(1000000);  // SysTick
    printf(".5s\n");
    MXC_Delay(500000);  // SysTick
    printf("10ms\n");
    MXC_Delay(10000);  // SysTick
    printf("1ms\n");
    MXC_Delay(1000);  // SysTick
    while (1) {
        printf("Done.\n");
    }
}
