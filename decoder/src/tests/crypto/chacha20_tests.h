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
 * Tag: 01a03a8fe66e467666c64dcc53c0cf99
 * Cipher: 6c05db5e173f5916c1f2b972ff832ffa8e96adaece2d1ab106fd1dcd5eef85ac2a3f6ef4af177f3220a8998594688f8f4a2b7db64099db6b480515135da8fbc1abb9f24dcf83
*/
const uint8_t CHACHA20_TEST_KEY[] = { 0x41, 0x89, 0x06, 0x5b, 0x77, 0xd5, 0x18, 0x4d, 0x81, 0xc8, 0xce, 0x38, 0x10, 0xca, 0x35, 0x8d, 
                                      0x56, 0x51, 0x44, 0x32, 0xc1, 0x67, 0x2f, 0x2f, 0x2d, 0xd0, 0xb1, 0x0b, 0x4d, 0x14, 0x7f, 0xb5 };
const uint8_t CHACHA20_TEST_NONCE[] = { 0x04, 0x1c, 0xaf, 0x4b, 0x50, 0xd0, 0xe4, 0xab, 0xf1, 0xe7, 0xbf, 0xcd };
const uint8_t CHACHA20_TEST_AD[] = "%this is some test additional data.";
const uint8_t CHACHA20_TEST_AD_LENGTH = 36;
const uint8_t CHACHA20_TEST_ENCRYPTED_CIPHER[] = { 0x6c, 0x05, 0xdb, 0x5e, 0x17, 0x3f, 0x59, 0x16, 0xc1, 0xf2, 0xb9, 0x72, 0xff, 0x83, 0x2f, 0xfa, 0x8e, 0x96, 0xad, 0xae, 0xce, 0x2d, 0x1a, 0xb1, 0x06, 0xfd, 0x1d, 0xcd, 0x5e, 0xef, 0x85, 0xac, 0x2a, 0x3f, 0x6e,
                                                   0xf4, 0xaf, 0x17, 0x7f, 0x32, 0x20, 0xa8, 0x99, 0x85, 0x94, 0x68, 0x8f, 0x8f, 0x4a, 0x2b, 0x7d, 0xb6, 0x40, 0x99, 0xdb, 0x6b, 0x48, 0x05, 0x15, 0x13, 0x5d, 0xa8, 0xfb, 0xc1, 0xab, 0xb9, 0xf2, 0x4d, 0xcf, 0x83 };
const uint8_t CHACHA20_TEST_PLAINTEXT[] = "This is some test plaintext. Secrets secrets secrets secrets secrets.";
const uint8_t CHACHA20_TEST_CIPHER_LENGTH = 70;
const uint8_t CHACHA20_TEST_TAG[] = { 0x01, 0xa0, 0x3a, 0x8f, 0xe6, 0x6e, 0x46, 0x76, 0x66, 0xc6, 0x4d, 0xcc, 0x53, 0xc0, 0xcf, 0x99 };

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
    uint8_t plaintext[CHACHA20_TEST_CIPHER_LENGTH];
    if (chacha20_decrypt(CHACHA20_TEST_KEY,
                         CHACHA20_TEST_ENCRYPTED_CIPHER,
                         CHACHA20_TEST_CIPHER_LENGTH,
                         CHACHA20_TEST_TAG,
                         CHACHA20_TEST_AD,
                         CHACHA20_TEST_AD_LENGTH,
                         CHACHA20_TEST_NONCE,
                         plaintext) != SUCCESS) { return ERROR; }

    if (memcmp(plaintext, CHACHA20_TEST_ENCRYPTED_CIPHER, CHACHA20_TEST_CIPHER_LENGTH) != 0) { 
        printf("Expected %*s, got %*s", CHACHA20_TEST_CIPHER_LENGTH, CHACHA20_TEST_PLAINTEXT, CHACHA20_TEST_CIPHER_LENGTH, plaintext);
        return ERROR;
    }
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