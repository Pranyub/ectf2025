#ifndef CHACHA20_TEST_H
#define CHACHA20_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../crypto/cc20p1305.h"
#include "../../constants.h"

// GROUND TRUTHS
const uint8_t CHACHA20_TEST_KEY[] = { 0x3c, 0x6e, 0x0b, 0x8a, 0x9c, 0x15, 0x22, 0x4a, 0x82, 0x28, 0xb9, 0xa9, 0x8c, 0xa1, 0x53, 0x1d };
const uint8_t CHACHA20_TEST_NONCE[] = { 0x04, 0x1c, 0xaf, 0x4b, 0x50, 0xd0, 0xe4, 0xab, 0xf1, 0xe7, 0xbf, 0xcd };
const uint8_t CHACHA20_TEST_AD[] = "%this is some test additional data.";
const uint8_t CHACHA20_TEST_AD_LENGTH = 36;
const uint8_t CHACHA20_TEST_ENCRYPTED_CIPHER[] = {};
const uint8_t CHACHA20_TEST_PLAINTEXT[] = "This is some test plaintext. Secrets secrets secrets secrets secrets.";
const uint8_t CHACHA20_TEST_CIPHER_LENGTH = 70;
const uint8_t CHACHA20_TEST_TAG[] = {};

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