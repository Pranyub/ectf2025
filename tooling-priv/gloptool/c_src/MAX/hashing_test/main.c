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
 *          MD5, SHA-1, SHA-256, and BLAKE2s.
 */

/***** Includes *****/
#include <stdio.h>
#include <memory.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include "mxc_device.h"
#include "mxc_sys.h"
#include "mxc_delay.h"
#include "tmr.h"

#include "include/md5/md5.h"
#include "include/sha1/sha1.h"
#include "include/sha256/sha256.h"
#include "include/blake2s/blake2s.h"

/***** Definitions *****/

/***** Globals *****/
char* INPUT_STRING = "19uR8uXz2lfZhUN0qKHCNLcbyJRTBJsB";
uint8_t MD5_HASH[] = {0x26, 0xdb, 0x08, 0xc4, 0x3c, 0x83, 0x9d, 0xa0, 0xd7, 0x95, 0x22, 0x9c, 0x14, 0xd7, 0xcd, 0x97};
uint8_t SHA1_HASH[] = {0xeb, 0xc1, 0x0a, 0x83, 0xc2, 0x02, 0x13, 0x69, 0xca, 0x45, 0xd4, 0xf5, 0x33, 0xbd, 0x86, 0x5e, 0xb8, 0x0f, 0x3c, 0xd4};
uint8_t SHA256_HASH[] = {0xda, 0xb6, 0x9f, 0x1c, 0x6a, 0x77, 0x0e, 0xac, 0x08, 0x28, 0xe6, 0x02, 0x0a, 0xe7, 0xb9, 0x1a, 0x08, 0x73, 0x6f, 0x26, 0x42, 0x39, 0xa1, 0x0c, 0x30, 0x0a, 0x67, 0x14, 0xcf, 0xf9, 0xce, 0x4f};
uint8_t BLAKE2S_HASH[] = {0x0a, 0x19, 0x19, 0x18, 0xb9, 0xac, 0x86, 0x05, 0xf6, 0xec, 0xd0, 0xbc, 0x22, 0x75, 0x61, 0x76, 0x78, 0xf1, 0x57, 0xf9, 0x2a, 0x00, 0x57, 0x76, 0xc6, 0xd8, 0xe6, 0x85, 0x72, 0x37, 0x45, 0x58};
mxc_tmr_unit_t TIME_UNITS = MXC_TMR_UNIT_MICROSEC;

/***** Functions *****/

// *****************************************************************************

/**
 * @brief computes the MD5 hash of a string, then compares it against the true value \param n number of times
 * @param input the input string to be hashed
 * @param compare the correct MD5 hash given the input (to be compared against the computed hash)
 * @param n the number of comparisons to do
 * @returns 0 on success, -1 on failed comparison
 */
int md5_hash(char* input, uint8_t* compare, int n);

/**
 * @brief computes the SHA-1 hash of a string, then compares it against the true value \param n number of times
 * @param input the input string to be hashed
 * @param compare the correct SHA-1 hash given the input (to be compared against the computed hash)
 * @param n the number of comparisons to do
 * @returns 0 on success, -1 on failed comparison
 */
int sha1_hash(char* input, uint8_t* compare, int n);

/**
 * @brief computes the SHA-256 hash of a string, then compares it against the true value \param n number of times
 * @param input the input string to be hashed
 * @param compare the correct SHA-256 hash given the input (to be compared against the computed hash)
 * @param n the number of comparisons to do
 * @returns 0 on success, -1 on failed comparison
 */
int sha256_hash(char* input, uint8_t* compare, int n);

/**
 * @brief computes the keyless 32-byte BLAKE2s hash of a string, then compares it against the true value \param n number of times
 * @param input the input string to be hashed
 * @param compare the correct BLAKE2s hash given the input (to be compared against the computed hash)
 * @param n the number of comparisons to do
 * @returns 0 on success, -1 on failed comparison
 */
int blake2s_hash(char* input, uint8_t* compare, int n);

int main(void)
{
    printf("\nHashing Test.\nInput data: %s\n", INPUT_STRING);

    while (1) {
        if (md5_hash(INPUT_STRING, MD5_HASH, 100000) == -1) {
            printf("ERROR: MD5 hash comparison failed.\n");
        }

        if (sha1_hash(INPUT_STRING, SHA1_HASH, 100000) == -1) {
            printf("ERROR: SHA-1 hash comparison failed.\n");
        }

        if (sha256_hash(INPUT_STRING, SHA256_HASH, 100000) == -1) {
            printf("ERROR: SHA-256 hash comparison failed.\n");
        }

        if (blake2s_hash(INPUT_STRING, BLAKE2S_HASH, 100000) == -1) {
            printf("ERROR: BLAKE2s hash comparison failed.\n");
        }
        MXC_Delay(1000000);  // 1s delay
    }
}

int md5_hash(char* input, uint8_t* compare, int n) {
    printf("\nTesting MD5.\n");
    uint8_t md5_result[16];
    uint32_t elapsed_time, fixed_time;
    MXC_TMR_SW_Start(MXC_TMR0);
    for (int k = 0; k < n; k++) {
        md5String(input, md5_result);
    }
    elapsed_time = MXC_TMR_SW_Stop(MXC_TMR0);  // max78000 documentation example says that this is the time in us, but on the riscv, it is way too low
    MXC_TMR_GetTime(MXC_TMR0, elapsed_time, &fixed_time, &TIME_UNITS);
    if (memcmp(md5_result, compare, 16) != 0) {
        return -1;
    };
    printf("%d MD5 hashes computed in %"PRIu32" microseconds.\n", n, elapsed_time);
    printf("%d MD5 hashes computed in %"PRIu32" microseconds.\n", n, fixed_time);
    return 0;
}

int sha1_hash(char* input, uint8_t* compare, int n) {
    printf("\nTesting SHA-1.\n");
    char sha1_result[21];  // not sure why the example is not 20
    uint32_t elapsed_time, fixed_time;
    MXC_TMR_SW_Start(MXC_TMR0);
    for (int k = 0; k < n; k++) {
        SHA1(sha1_result, input, strlen(input));
    }
    elapsed_time = MXC_TMR_SW_Stop(MXC_TMR0);
    MXC_TMR_GetTime(MXC_TMR0, elapsed_time, &fixed_time, &TIME_UNITS);
    if (memcmp(sha1_result, compare, 16) != 0) {
        return -1;
    };
    printf("%d SHA-1 hashes computed in %"PRIu32" microseconds.\n", n, elapsed_time);
    printf("%d SHA-1 hashes computed in %"PRIu32" microseconds.\n", n, fixed_time);
    return 0;
}

int sha256_hash(char* input, uint8_t* compare, int n) {
    printf("\nTesting SHA-256.\n");
    uint8_t sha256_result[32];
    uint32_t elapsed_time, fixed_time;
    SHA256_CTX ctx;
    MXC_TMR_SW_Start(MXC_TMR0);
    for (int k = 0; k < n; k++) {
        sha256_init(&ctx);
        sha256_update(&ctx, input, strlen(input));
        sha256_final(&ctx, sha256_result);
        // for (int i = 0; i < 32; i++) {
        //     printf("%02X %02X\n", sha256_result[i], compare[i]);
        // }
        // printf("\n");
    }
    elapsed_time = MXC_TMR_SW_Stop(MXC_TMR0);
    MXC_TMR_GetTime(MXC_TMR0, elapsed_time, &fixed_time, &TIME_UNITS);
    if (memcmp(sha256_result, compare, 32) != 0) {
        return -1;
    }
    printf("%d SHA-256 hashes computed in %"PRIu32" microseconds.\n", n, elapsed_time);
    printf("%d SHA-256 hashes computed in %"PRIu32" microseconds.\n", n, fixed_time);
    return 0;
}

int blake2s_hash(char* input, uint8_t* compare, int n) {
    printf("\nTesting BLAKE2s.\n");
    uint8_t blake2s_result[32];
    uint32_t elapsed_time, fixed_time;
    MXC_TMR_SW_Start(MXC_TMR0);
    for (int k = 0; k < n; k++) {
        blake2s(blake2s_result, 32, NULL, 0, input, strlen(input));
    }
    elapsed_time = MXC_TMR_SW_Stop(MXC_TMR0);
    MXC_TMR_GetTime(MXC_TMR0, elapsed_time, &fixed_time, &TIME_UNITS);
    if (memcmp(blake2s_result, compare, 32) != 0) {
        return -1;
    }
    printf("%d BLAKE2s hashes computed in %"PRIu32" microseconds.\n", n, elapsed_time);
    printf("%d BLAKE2s hashes computed in %"PRIu32" microseconds.\n", n, fixed_time);
    return 0;
}
