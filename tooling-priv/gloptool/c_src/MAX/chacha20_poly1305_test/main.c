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
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "mxc_device.h"
#include "trng.h"
#include "tmr.h"

#include "include/chacha20_poly1305/portable8439.h"

/***** Definitions *****/
// #define RFC_8439_TAG_SIZE (16)
// #define RFC_8439_KEY_SIZE (32)
// #define RFC_8439_NONCE_SIZE (12)

/***** Globals *****/
const char INPUT_STRING[] = "Hello world! Hello world! Hello world! Hello world! Hello world! Hello world! Hello world!";  // 91 bytes, including null terminator
const char* AAD = NULL;
const uint8_t KEY[] = { 0x3c, 0x6e, 0x0b, 0x8a, 0x9c, 0x15, 0x22, 0x4a, 0x82, 0x28, 0xb9, 0xa9, 0x8c, 0xa1, 0x53, 0x1d, 
                        0x3c, 0x6e, 0x0b, 0x8a, 0x9c, 0x15, 0x22, 0x4a, 0x82, 0x28, 0xb9, 0xa9, 0x8c, 0xa1, 0x53, 0x1d };
const uint8_t KEY_LENGTH = 32;  // 256 bits
const uint8_t TAG_LENGTH = 16;
const uint8_t NONCE_SIZE = 12;
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
    int warning = 0;
    MXC_TRNG_Init();
    uint8_t NONCE[NONCE_SIZE];
    MXC_TRNG_Random(NONCE, NONCE_SIZE);  // generate IV from random bytes
    int length = strlen(INPUT_STRING) + 1;
    uint8_t encrypted[length];
    uint8_t encryption_tag[TAG_LENGTH];
    int aad_length = 0;
    if (AAD != NULL) {
        aad_length = strlen(AAD) + 1;
    }
    
    // encryption
    uint8_t encryption_buffer[length + TAG_LENGTH];
    size_t encryption_cipher_size = portable_chacha20_poly1305_encrypt(encryption_buffer, KEY, NONCE, (uint8_t*)AAD, aad_length, (uint8_t*)INPUT_STRING, length);
    if (encryption_cipher_size == -1) {  // encryption_cipher_size should be the size of the amount of data written to the buffer, or -1 on error
        printf("Error encrypting.\n");
        return -1;
    }
    
    memcpy(encrypted, encryption_buffer, length);
    memcpy(encryption_tag, encryption_buffer + length, TAG_LENGTH);

    char encrypted_string[length * 2 + 1];
    bytes_to_string(encrypted, length, encrypted_string);

    char encryption_tag_string[TAG_LENGTH * 2 + 1];
    bytes_to_string(encryption_tag, TAG_LENGTH, encryption_tag_string);
    printf("Input string: %s\n", INPUT_STRING);
    printf("Encrypted cipher: %s\n", encrypted_string);
    printf("Encryption Tag: %s\n\n", encryption_tag_string);

    // // decryption
    uint8_t decrypted[length];
    uint8_t decryption_tag[TAG_LENGTH];

    size_t decryption_cipher_size = portable_chacha20_poly1305_decrypt(decrypted, KEY, NONCE, (uint8_t*)AAD, aad_length, encryption_buffer, encryption_cipher_size);

    if (decryption_cipher_size == -1) {
        printf("Error decrypting.\n");
        return -1;
    }

    char decryption_tag_string[TAG_LENGTH * 2 + 1];
    bytes_to_string(decryption_tag, TAG_LENGTH, decryption_tag_string);
    printf("Input encryption: %s\n", encrypted_string);
    printf("Decrypted cipher: %s\n", (char*)decrypted);
    // printf("Decryption Tag: %s\n\n", decryption_tag_string);

    // // testing the tag
    // printf("Checking the tag...\n");
    // if (memcmp(encryption_tag, decryption_tag, TAG_LENGTH) != 0) {
    //     printf("WARNING: the encryption tag DOES NOT match the decryption tag when it SHOULD!\n\n");
    //     warning = 1;
    // } else {
    //     printf("Passed.\n");
    // }

    // // testing modification of encrypted cipher
    // printf("Testing a modified encrypted cipher...\n");
    // encrypted[0] ^= 0b1;  // flip the first bit of the first byte
    // gcm_crypt_and_tag(&ctx, 0, iv, 12, (uint8_t*)AEAD, aead_length, encrypted, decrypted, length, decryption_tag, TAG_LENGTH);  // decrypting
    // if (memcmp(encryption_tag, decryption_tag, TAG_LENGTH) == 0) {
    //     printf("WARNING: the encryption tag DOES match the decryption tag when it SHOULD NOT.\n\n");
    //     warning = 1;
    // } else {
    //     printf("Passed.\n\n");
    // }

    // if (warning != 0) {
    //     printf("WARNING: one or more tests have failed.\n");
    // } else {
    //     printf("All tests passed successfully.\n\n");
    // }

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


    printf("Done.\n");
}

void bytes_to_string(const uint8_t* input, int length, char* output) {
    for (int i = 0; i < length; i++) {
        sprintf(output + i, "%02x", input[i]);
    }
    output[length * 2 + 1] = '\n';
}
