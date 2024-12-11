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
 * @brief   AES test
 * @details This example uses encrypts and decrypts a message using AES.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_device.h"
#include "board.h"
#include "dma.h"
#include "aes.h"

/***** Definitions *****/

/***** Globals *****/

/***** Functions *****/

int AES_encrypt( input, mxc_aes_keys_t keysize) {
    request.length = strlen(input) + 1;
    request.inputData = 
}

int main(void)
{
    mxc_aes_req_t request;
    char input[] = {"Hello World!"};

    printf("AES EBC test with \"%s\"\n", input);

    printf("128 bits.");

    printf("Encrypted: ");
    printf("Decrypted: ");

    printf("192 bits.");

    printf("Encrypted: ");
    printf("Decrypted: ");

    printf("256 bits.")
    printf("Encrypted: ");
    printf("Decrypted: ");

    while (1) {}
}
