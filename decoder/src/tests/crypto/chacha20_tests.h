#ifndef CHACHA20_TEST_H
#define CHACHA20_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../crypto/cc20p1305.h"
#include "../../constants.h"

// GROUND TRUTHS
/*
 * Nonce: 041caf4b50d0e4abf1e7bfcd
 * Tag: 905646ae2b535daf093febd4502e89e5
 * Cipher: e3b3b07e46913eba17f03f33ec1a9e37ed2ac4a3d2392dd7444ec0dc96b7be4b427ffda82ad9240c5d34cc578eb4ea1259074e8948d991b0c1eb595da31a1f8b15334fec4f7d
*/
const uint8_t CHACHA20_TEST_KEY[] = { 0x41, 0x89, 0x06, 0x5b, 0x77, 0xd5, 0x18, 0x4d, 0x81, 0xc8, 0xce, 0x38, 0x10, 0xca, 0x35, 0x8d, 
                                      0x56, 0x51, 0x44, 0x32, 0xc1, 0x67, 0x2f, 0x2f, 0x2d, 0xd0, 0xb1, 0x0b, 0x4d, 0x14, 0x7f, 0xb5 };
const uint8_t CHACHA20_TEST_NONCE[] = { 0x04, 0x1c, 0xaf, 0x4b, 0x50, 0xd0, 0xe4, 0xab, 0xf1, 0xe7, 0xbf, 0xcd };
const uint8_t CHACHA20_TEST_AD[] = "%this is some test additional data.";
const uint8_t CHACHA20_TEST_AD_LENGTH = 36;
const uint8_t CHACHA20_TEST_ENCRYPTED_CIPHER[] = { 0xe3, 0xb3, 0xb0, 0x7e, 0x46, 0x91, 0x3e, 0xba, 0x17, 0xf0, 0x3f, 0x33, 0xec, 0x1a, 0x9e, 0x37, 0xed, 0x2a, 0xc4, 0xa3, 0xd2, 0x39, 0x2d, 0xd7, 0x44, 0x4e, 0xc0, 0xdc, 0x96, 0xb7, 0xbe, 0x4b, 0x42, 0x7f, 0xfd,
                                                   0xa8, 0x2a, 0xd9, 0x24, 0x0c, 0x5d, 0x34, 0xcc, 0x57, 0x8e, 0xb4, 0xea, 0x12, 0x59, 0x07, 0x4e, 0x89, 0x48, 0xd9, 0x91, 0xb0, 0xc1, 0xeb, 0x59, 0x5d, 0xa3, 0x1a, 0x1f, 0x8b, 0x15, 0x33, 0x4f, 0xec, 0x4f, 0x7d };
const uint8_t CHACHA20_TEST_PLAINTEXT[] = "This is some test plaintext. Secrets secrets secrets secrets secrets.";
const uint8_t CHACHA20_TEST_CIPHER_LENGTH = 70;
const uint8_t CHACHA20_TEST_TAG[] = { 0x90, 0x56, 0x46, 0xae, 0x2b, 0x53, 0x5d, 0xaf, 0x09, 0x3f, 0xeb, 0xd4, 0x50, 0x2e, 0x89, 0xe5 };

static int chacha20_test_decrypt_normal(void);
static int chacha20_test_decrypt_incorrect_key(void);
static int chacha20_test_decrypt_incorrect_incorrect_nonce(void);
static int chacha20_test_decrypt_incorrect_incorrect_ad(void);
static int chacha20_test_decrypt_incorrect_cipher(void);
static int chacha20_test_decrypt_incorrect_tag(void);
static int chacha20_test_decrypt_incorrect_cipher_size(void);  // the actual cipher size itself
static int chacha20_test_decrypt_incorrect_cipher_length(void);  // as the parameter
static int chacha20_test_decrypt_incorrect_ad_size(void);  // the actual ad size itself
static int chacha20_test_decrypt_incorrect_ad_length(void);  // as the parameter

/**
 * @brief runs all tests
 * @returns SUCCESS on success, ERROR on error
 */
int chacha20_test_all(void) {
    int success = SUCCESS;
    printf("Starting tests...\n");
    if (chacha20_test_decrypt_normal() == ERROR) {
        printf("ChaCha20-Poly1305 failed the normal decryption test.\n");
        success = ERROR;
    }

    if (chacha20_test_decrypt_incorrect_key() == ERROR) {
        printf("ChaCha20-Poly1305 failed the incorrect key decryption test.\n");
        success = ERROR;
    }
    
    if (chacha20_test_decrypt_incorrect_incorrect_nonce() == ERROR) {
        printf("ChaCha20-Poly1305 failed the incorrect nonce decryption test.\n");
        success = ERROR;
    }

    if (chacha20_test_decrypt_incorrect_incorrect_ad() == ERROR) {
        printf("ChaCha20-Poly1305 failed the incorrect ad decryption test.\n");
        success = ERROR;
    }
    
    if (chacha20_test_decrypt_incorrect_cipher() == ERROR) {
        printf("ChaCha20-Poly1305 failed the cipher decryption test.\n");
        success = ERROR;
    }
    
    if (chacha20_test_decrypt_incorrect_tag() == ERROR) {
        printf("ChaCha20-Poly1305 failed the tag decryption test.\n");
        success = ERROR;
    }
    
    if (chacha20_test_decrypt_incorrect_cipher_size() == ERROR) {
        printf("ChaCha20-Poly1305 failed the cipher size decryption test.\n");
        success = ERROR;
    }
    
    if (chacha20_test_decrypt_incorrect_cipher_length() == ERROR) {
        printf("ChaCha20-Poly1305 failed the cipher length decryption test.\n");
        success = ERROR;
    }

    if (chacha20_test_decrypt_incorrect_ad_size() == ERROR) {
        printf("ChaCha20-Poly1305 failed the ad size decryption test.\n");
        success = ERROR;
    }

    if (chacha20_test_decrypt_incorrect_ad_length() == ERROR) {
        printf("ChaCha20-Poly1305 failed the ad length decryption test.\n");
        success = ERROR;
    }

    printf("Testing complete.\n");
    return success;
}

static int chacha20_test_decrypt_normal(void) {
    uint8_t* plaintext = (uint8_t*)malloc(sizeof(uint8_t) * CHACHA20_TEST_CIPHER_LENGTH);
    if (chacha20_decrypt(CHACHA20_TEST_KEY,
                         CHACHA20_TEST_ENCRYPTED_CIPHER,
                         CHACHA20_TEST_CIPHER_LENGTH,
                         CHACHA20_TEST_TAG,
                         CHACHA20_TEST_AD,
                         CHACHA20_TEST_AD_LENGTH,
                         CHACHA20_TEST_NONCE,
                         plaintext) != SUCCESS) { return ERROR; }

    if (memcmp(plaintext, CHACHA20_TEST_ENCRYPTED_CIPHER, CHACHA20_TEST_CIPHER_LENGTH) != 0) { return ERROR; }
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_key(void) {
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_incorrect_nonce(void) {
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_incorrect_ad(void) {
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_cipher(void) {
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_tag(void) {
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_cipher_size(void) {
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_cipher_length(void) {
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_ad_size(void) {
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_ad_length(void) {
    return SUCCESS;
}

#endif