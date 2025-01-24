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
#include "include/md5/md5.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_device.h"
#include "mxc_sys.h"

/***** Definitions *****/
#define INPUT_DATA "19uR8uXz2lfZhUN0qKHCNLcbyJRTBJsB"  // 256-bit string
#define MD5_HASH "d9e10fcefa95a8b139b98c587695920a"  // in hex
#define SHA1_HASH "a9236393ad539fa2208d62924db3f034d033f67c"  // in hex
#define SHA256_HASH "d503b11caeca4ed5b3173e4392322644f4b9a6cd2707bae89891a5e7df0f3133"  // in hex

/***** Globals *****/

/***** Functions *****/

// *****************************************************************************
int main(void)
{
    uint8_t md5_result[16];
    char* input = INPUT_DATA;

    printf("Hashing Test.\nInput data: %s\n", input);

    printf("Testing MD5.\n");
    md5String(input, md5_result);
    printf("Hash output: %16x", md5_result);
    
    printf("Hash succeeded.\n");}

