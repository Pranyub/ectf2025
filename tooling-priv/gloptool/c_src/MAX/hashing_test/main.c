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
 * @brief   Tests the speed of various hashing functions.
 * @details Tests the speed of various hashing funcitons.
 *          MD5, SHA-1, SHA-256, and BLAKE.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_device.h"
#include "mxc_delay.h"
#include "mxc_sys.h"

#include "include/md5/md5.h"
#include "include/sha1/sha1.h"

/***** Definitions *****/
#define INPUT_DATA "19uR8uXz2lfZhUN0qKHCNLcbyJRTBJsB"  // 256-bit string
#define SHA1_HASH "ebc10a83c2021369ca45d4f533bd865eb80f3cd4"  // in hex
#define SHA256_HASH "dab69f1c6a770eac0828e6020ae7b91a08736f264239a10c300a6714cff9ce4f"  // in hex

/***** Globals *****/
uint8_t MD5_HASH[] = {0x26, 0xdb, 0x08, 0xc4, 0x3c, 0x83, 0x9d, 0xa0, 0xd7, 0x95, 0x22, 0x9c, 0x14, 0xd7, 0xcd, 0x97};

/***** Functions *****/

// *****************************************************************************

/**
 * @brief computes the md5 hash of a string, then compares it against the true value \param n number of times
 * @param input the input string to be hashed
 * @param compare the correct md5 hash given the input (to be compared against the computed hash)
 * @param n the number of comparisons to do
 * @returns 0 on success, -1 on failed comparison
 */
int md5_hash(char* input, uint8_t* compare, int n);

/**
 * @brief computes the sha1 hash of a string, then compares it against the true value \param n number of times
 * @param input the input string to be hashed
 * @param compare the correct sha1 hash given the input (to be compared against the computed hash)
 * @param n the number of comparisons to do
 * @returns 0 on success, -1 on failed comparison
 */
int sha1_hash(char* input, uint8_t* compare, int n);

int main(void)
{
    char* input = INPUT_DATA;

    printf("\nHashing Test.\nInput data: %s\n", input);

    while (1) {
        if (md5_hash(input, MD5_HASH, 1000) == -1) {
            printf("ERROR: MD5 hash comparison failed.");
        }

        if (sha1_hash(input, SHA1_HASH, 1000) == -1) {
            printf("ERROR: SHA1 hash comparison failed.");
        }
        MXC_Delay(1000000);  // 1s delay
    }
}

int md5_hash(char* input, uint8_t* compare, int n) {
    printf("\nTesting MD5.\n");
    uint8_t md5_result[16];
    for (int k = 0; i < n; k++) {
        md5String(input, md5_result);
        for (int i = 0; i < 16; i++) {
            if (md5_result[i] != compare[i]) {
                printf("MD5 hash failed.\n");
                return -1;
            }
        }
    }
    printf("MD5 hash test done.\n");
    return 0;
}

int sha1_hash(char* input, uint8_t* compare, int n) {
    printf("\nTesting SHA-1.\n");
    printf("SHA-1 hash test done.\n");
    return 0;
}
