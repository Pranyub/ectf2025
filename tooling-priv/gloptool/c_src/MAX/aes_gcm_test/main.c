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
 * command to flash: sudo openocd -f interface/cmsis-dap.cfg -f target/max78000.cfg -c "init; reset halt; max32xxx mass_erase 0; program {aes.elf} verify exit"
 * command to run: sudo openocd -f interface/cmsis-dap.cfg -f target/max78000.cfg -c "init"
*/
/**
 * @file    main.c
 * @brief   Input test
 * @details This example uses the UART to say hi, then echo inputs.
 */

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "mxc_device.h"
#include "trng.h"
#include "tmr.h"

#include "include/aes_gcm/gcm.h"

/***** Definitions *****/

/***** Globals *****/
const char INPUT_STRING[] = "Hello world! Hello world! Hello world! Hello world! Hello world! Hello world! Hello world!";  // 91 bytes, including null terminator
const char* AEAD = NULL;
const uint8_t KEY[] = { 0x3c, 0x6e, 0x0b, 0x8a, 0x9c, 0x15, 0x22, 0x4a, 0x82, 0x28, 0xb9, 0xa9, 0x8c, 0xa1, 0x53, 0x1d };
const uint8_t KEY_LENGTH = 16;  // 128 bits
const uint8_t TAG_LENGTH = 16;
const int NUMBER_OF_TRIALS = 100000;

/***** Functions *****/

// *****************************************************************************

/**
 * @brief converts an array of bytes to a hex string
 * @param input pointer to the array of bytes
 * @param length the length of the array of bytes: \p input
 * @param output a pointer to the string to store the string. Should be at least twice \p length + 1
 */
void bytes_to_string(const uint8_t* input, int length, char* output);

int main(void)
{
    printf("Starting non-hardware accelerated tests.\n");
    int warning = 0;
    MXC_TRNG_Init();
    gcm_initialize();
    gcm_context ctx;
    if (gcm_setkey(&ctx, KEY, KEY_LENGTH) != 0) {
        printf("ERROR: unable to set GCM key.\n");
        return 1;
    }


    uint8_t iv[12];
    MXC_TRNG_Random(iv, 12);  // generate IV from random bytes
    int length = strlen(INPUT_STRING) + 1;
    uint8_t encrypted[length];
    uint8_t encryption_tag[TAG_LENGTH];
    int aead_length = 0;
    if (AEAD != NULL) {
        aead_length = strlen(AEAD);
    }
    
    // encryption
    gcm_crypt_and_tag(&ctx, 1, iv, 12, (uint8_t*)AEAD, aead_length, (uint8_t*)INPUT_STRING, encrypted, length, encryption_tag, TAG_LENGTH);

    char encrypted_string[length * 2 + 1];
    bytes_to_string(encrypted, length, encrypted_string);

    char encryption_tag_string[TAG_LENGTH * 2 + 1];
    bytes_to_string(encryption_tag, TAG_LENGTH, encryption_tag_string);
    printf("Input string: %s\n", INPUT_STRING);
    printf("Encrypted cipher: %s\n", encrypted_string);
    printf("Encryption Tag: %s\n\n", encryption_tag_string);

    // decryption
    uint8_t decrypted[length];
    uint8_t decryption_tag[TAG_LENGTH];
    gcm_crypt_and_tag(&ctx, 0, iv, 12, (uint8_t*)AEAD, aead_length, encrypted, decrypted, length, decryption_tag, TAG_LENGTH);

    char decryption_tag_string[TAG_LENGTH * 2 + 1];
    bytes_to_string(decryption_tag, TAG_LENGTH, decryption_tag_string);
    printf("Input encryption: %s\n", encrypted_string);
    printf("Decrypted cipher: %s\n", (char*)decrypted);
    printf("Decryption Tag: %s\n\n", decryption_tag_string);

    // testing the tag
    printf("Checking the tag...\n");
    if (memcmp(encryption_tag, decryption_tag, TAG_LENGTH) != 0) {
        printf("WARNING: the encryption tag DOES NOT match the decryption tag when it SHOULD!\n\n");
        warning = 1;
    } else {
        printf("Passed.\n");
    }

    // testing modification of encrypted cipher
    printf("Testing a modified encrypted cipher...\n");
    encrypted[0] ^= 0b1;  // flip the first bit of the first byte
    gcm_crypt_and_tag(&ctx, 0, iv, 12, (uint8_t*)AEAD, aead_length, encrypted, decrypted, length, decryption_tag, TAG_LENGTH);  // decrypting
    if (memcmp(encryption_tag, decryption_tag, TAG_LENGTH) == 0) {
        printf("WARNING: the encryption tag DOES match the decryption tag when it SHOULD NOT.\n\n");
        warning = 1;
    } else {
        printf("Passed.\n\n");
    }

    if (warning != 0) {
        printf("WARNING: one or more tests have failed.\n");
    } else {
        printf("All tests passed successfully.\n\n");
    }

    // measuring time of encryption
    // printf("Testing %d encryptions...\n", NUMBER_OF_TRIALS);
    // uint32_t elapsed_time;
    // MXC_TMR_SW_Start(MXC_TMR0);
    // for (int i = 0; i < NUMBER_OF_TRIALS; i++) {
    //     gcm_crypt_and_tag(&ctx, 1, iv, 12, (uint8_t*)AEAD, aead_length, (uint8_t*)INPUT_STRING, encrypted, length, encryption_tag, TAG_LENGTH);  // encrypting
    // }
    // elapsed_time = MXC_TMR_SW_Stop(MXC_TMR0);
    // printf("%d encryptions took %"PRIu32" microseconds.\n\n", NUMBER_OF_TRIALS, elapsed_time);

    // measuring time of decyrption
    // printf("Testing %d decryptions...\n", NUMBER_OF_TRIALS);
    // MXC_TMR_SW_Start(MXC_TMR0);
    // for (int i = 0; i < NUMBER_OF_TRIALS; i++) {
    //     gcm_crypt_and_tag(&ctx, 0, iv, 12, (uint8_t*)AEAD, aead_length, encrypted, decrypted, length, decryption_tag, TAG_LENGTH);  // decrypting
    // }
    // elapsed_time = MXC_TMR_SW_Stop(MXC_TMR0);
    // printf("%d decryptions took %"PRIu32" microseconds.\n\n", NUMBER_OF_TRIALS, elapsed_time);


    // hardware accelerated portion
    printf("\nStarting hardware accelerated tests\n");
    warning = 0;
    // encryption
    MAX_gcm_crypt_and_tag(&ctx, 1, iv, 12, (uint8_t*)AEAD, aead_length, (uint8_t*)INPUT_STRING, encrypted, length, encryption_tag, TAG_LENGTH);  // encrypting
    bytes_to_string(encrypted, length, encrypted_string);
    bytes_to_string(encryption_tag, TAG_LENGTH, encryption_tag_string);
    printf("Input string: %s\n", INPUT_STRING);
    printf("Encrypted cipher: %s\n", encrypted_string);
    printf("Encryption Tag: %s\n\n", encryption_tag_string);

    // decryption
    gcm_crypt_and_tag(&ctx, 0, iv, 12, (uint8_t*)AEAD, aead_length, encrypted, decrypted, length, decryption_tag, TAG_LENGTH);
    bytes_to_string(decryption_tag, TAG_LENGTH, decryption_tag_string);
    printf("Input encryption: %s\n", encrypted_string);
    printf("Decrypted cipher: %s\n", (char*)decrypted);
    printf("Decryption Tag: %s\n\n", decryption_tag_string);


    printf("Done.\n");
}

void bytes_to_string(const uint8_t* input, int length, char* output) {
    for (int i = 0; i < length; i++) {
        sprintf(output + i, "%02x", input[i]);
    }
    output[length * 2 + 1] = '\n';
}
