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
 * @brief   Input test
 * @details This example uses the UART to say hi, then echo inputs.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include "mxc_device.h"
#include "include/aes_gcm/gcm.h"

/***** Definitions *****/

/***** Globals *****/
char INPUT_STRING[] = "Hello world! Hello world! Hello world! Hello world! Hello world! Hello world! Hello world!";
uint8_t KEY[] = { 0x3c, 0x6e, 0x0b, 0x8a, 0x9c, 0x15, 0x22, 0x4a, 0x82, 0x28, 0xb9, 0xa9, 0x8c, 0xa1, 0x53, 0x1d };

/***** Functions *****/

// *****************************************************************************
int main(void)
{
    while(1) {
        printf("Hello");
    }
}
