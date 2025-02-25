#ifndef CHACHA20_TEST_H
#define CHACHA20_TEST_H

#include <stdio.h>
#include <string.h>

#include "simple_flash.h"
#include "host_messaging.h"
#include "simple_uart.h"

#include "crypto.h"

// GROUND TRUTHS
/*
 * Nonce: 041caf4b50d0e4abf1e7bfcd
 * Tag: 663f30e54bc4e7d682ea89a765b0465e
 * Cipher: b7349ba251b3a4cd55773387c12e14041510162b95aa6d3e90c8641e6601e68ca3ff666f9d1d20016d1f24f51a795e33d762c190d4949c239a28b887212d9b7dbbffadbaba27
*/
static const uint8_t CHACHA20_TEST_KEY[] = { 0x41, 0x89, 0x06, 0x5b, 0x77, 0xd5, 0x18, 0x4d, 0x81, 0xc8, 0xce, 0x38, 0x10, 0xca, 0x35, 0x8d, 
                                      0x56, 0x51, 0x44, 0x32, 0xc1, 0x67, 0x2f, 0x2f, 0x2d, 0xd0, 0xb1, 0x0b, 0x4d, 0x14, 0x7f, 0xb5 };
static const uint8_t CHACHA20_TEST_NONCE[] = { 0x04, 0x1c, 0xaf, 0x4b, 0x50, 0xd0, 0xe4, 0xab, 0xf1, 0xe7, 0xbf, 0xcd };
static const uint8_t CHACHA20_TEST_AD[] = "%this is some test additional data.";
static const uint8_t CHACHA20_TEST_AD_LENGTH = 36;
static const uint8_t CHACHA20_TEST_ENCRYPTED_CIPHER[] = { 0xb7, 0x34, 0x9b, 0xa2, 0x51, 0xb3, 0xa4, 0xcd, 0x55, 0x77, 0x33, 0x87, 0xc1, 0x2e, 0x14, 0x04, 0x15, 0x10, 0x16, 0x2b, 0x95, 0xaa, 0x6d, 0x3e, 0x90, 0xc8, 0x64, 0x1e, 0x66, 0x01, 0xe6, 0x8c, 0xa3, 0xff, 0x66,
                                                   0x6f, 0x9d, 0x1d, 0x20, 0x01, 0x6d, 0x1f, 0x24, 0xf5, 0x1a, 0x79, 0x5e, 0x33, 0xd7, 0x62, 0xc1, 0x90, 0xd4, 0x94, 0x9c, 0x23, 0x9a, 0x28, 0xb8, 0x87, 0x21, 0x2d, 0x9b, 0x7d, 0xbb, 0xff, 0xad, 0xba, 0xba, 0x27 };
static const uint8_t CHACHA20_TEST_PLAINTEXT[] = "This is some test plaintext. Secrets secrets secrets secrets secrets.";
static const uint8_t CHACHA20_TEST_CIPHER_LENGTH = 70;
static const uint8_t CHACHA20_TEST_TAG[] = { 0x66, 0x3f, 0x30, 0xe5, 0x4b, 0xc4, 0xe7, 0xd6, 0x82, 0xea, 0x89, 0xa7, 0x65, 0xb0, 0x46, 0x5e };

static int chacha20_test_decrypt_normal(void);
static int chacha20_test_decrypt_incorrect_key(void);
static int chacha20_test_decrypt_incorrect_nonce(void);
static int chacha20_test_decrypt_incorrect_ad(void);
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

    print_debug("Performing chacha20 tests...\n");
    if (chacha20_test_decrypt_normal() == SUCCESS) {
        print_debug("Passed normal decryption test.\n");
    } else {
        print_debug("Failed normal decyrption test.\n");
    }

    if (chacha20_test_decrypt_incorrect_key() == SUCCESS) {
        print_debug("Passed the incorrect key decyrption test.\n");
    } else {
        print_debug("Failed the incorrect key decryption test.\n");
    }

    if (chacha20_test_decrypt_incorrect_nonce() == SUCCESS) {
        print_debug("Passed the incorrect nonce decryption test.\n");
    } else {
        print_debug("Failed the incorrect nonce decryption test.\n");
    }

    if (chacha20_test_decrypt_incorrect_ad() == SUCCESS) {
        print_debug("Passed the incorrect ad decryption test.\n");
    } else {
        print_debug("Failed the incorrect ad decryption test.\n");
    }
    
    if (chacha20_test_decrypt_incorrect_cipher() == SUCCESS) {
        print_debug("Passed the incorrect cipher decryption test.\n");
    } else {
        print_debug("Failed the incorrect cipher decryption test.\n");
    }
    
    if (chacha20_test_decrypt_incorrect_tag() == SUCCESS) {
        print_debug("Passed the incorrect tag decryption test.\n");
    } else {
        print_debug("Failed the incorrect tag decryption test.\n");
    }
    
    if (chacha20_test_decrypt_incorrect_cipher_size() == SUCCESS) {
        print_debug("Passed the incorrect cipher size decryption test.\n");
    } else {
        print_debug("Failed the incorrect cipher size decryption test.\n");
    }
    
    if (chacha20_test_decrypt_incorrect_cipher_length() == SUCCESS) {
        print_debug("Passed the incorrect cipher length parameter decryption test.\n");
    } else {
        print_debug("Failed the incorrect cipher length parameter decryption test.\n");
    }

    if (chacha20_test_decrypt_incorrect_ad_size() == SUCCESS) {
        print_debug("Passed the incorrect ad size decryption test.\n");
    } else {
        print_debug("Failed the incorrect ad size decryption test.\n");
    }

    if (chacha20_test_decrypt_incorrect_ad_length() == SUCCESS) {
        print_debug("Passed the incorrect ad length parameter decryption test.\n");
    } else {
        print_debug("Failed the incorrect ad length parameter decryption test.\n");
    }
    
    print_debug("Finished chacha20 tests.\n");
    return SUCCESS;
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

    int memcmp_ret;
    if ((memcmp_ret = memcmp(plaintext, CHACHA20_TEST_PLAINTEXT, CHACHA20_TEST_CIPHER_LENGTH)) != 0) { 
        print_debug("plaintext mismatch.\n");
        return ERROR;
    }
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_key(void) {
    uint8_t plaintext[CHACHA20_TEST_CIPHER_LENGTH];
    uint8_t incorrect_key[CHACHA_KEY_SIZE];
    memcpy(incorrect_key, CHACHA20_TEST_KEY, CHACHA_KEY_SIZE);
    incorrect_key[CHACHA_KEY_SIZE / 2] ^= 1 << 4;
    if (chacha20_decrypt(incorrect_key,
                         CHACHA20_TEST_ENCRYPTED_CIPHER,
                         CHACHA20_TEST_CIPHER_LENGTH,
                         CHACHA20_TEST_TAG,
                         CHACHA20_TEST_AD,
                         CHACHA20_TEST_AD_LENGTH,
                         CHACHA20_TEST_NONCE,
                         plaintext) == SUCCESS) { return ERROR; }
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_nonce(void) {
    uint8_t plaintext[CHACHA20_TEST_CIPHER_LENGTH];
    uint8_t incorrect_nonce[NONCE_SIZE];
    memcpy(incorrect_nonce, CHACHA20_TEST_NONCE, NONCE_SIZE);
    incorrect_nonce[NONCE_SIZE / 2] ^= 1 << 4;
    if (chacha20_decrypt(CHACHA20_TEST_KEY,
                         CHACHA20_TEST_ENCRYPTED_CIPHER,
                         CHACHA20_TEST_CIPHER_LENGTH,
                         CHACHA20_TEST_TAG,
                         CHACHA20_TEST_AD,
                         CHACHA20_TEST_AD_LENGTH,
                         incorrect_nonce,
                         plaintext) == SUCCESS) { return ERROR; }
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_ad(void) {
    uint8_t plaintext[CHACHA20_TEST_CIPHER_LENGTH];
    uint8_t incorrect_ad[CHACHA20_TEST_AD_LENGTH];
    memcpy(incorrect_ad, CHACHA20_TEST_AD, CHACHA20_TEST_AD_LENGTH);
    incorrect_ad[CHACHA20_TEST_AD_LENGTH / 2] ^= 1 << 4;
    if (chacha20_decrypt(CHACHA20_TEST_KEY,
                         CHACHA20_TEST_ENCRYPTED_CIPHER,
                         CHACHA20_TEST_CIPHER_LENGTH,
                         CHACHA20_TEST_TAG,
                         incorrect_ad,
                         CHACHA20_TEST_AD_LENGTH,
                         CHACHA20_TEST_NONCE,
                         plaintext) == SUCCESS) { return ERROR; }
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_cipher(void) {
    uint8_t plaintext[CHACHA20_TEST_CIPHER_LENGTH];
    uint8_t incorrect_cipher[CHACHA20_TEST_CIPHER_LENGTH];
    memcpy(incorrect_cipher, CHACHA20_TEST_ENCRYPTED_CIPHER, CHACHA20_TEST_CIPHER_LENGTH);
    incorrect_cipher[CHACHA20_TEST_CIPHER_LENGTH / 2] ^= 1 << 4;
    if (chacha20_decrypt(CHACHA20_TEST_KEY,
                         incorrect_cipher,
                         CHACHA20_TEST_CIPHER_LENGTH,
                         CHACHA20_TEST_TAG,
                         CHACHA20_TEST_AD,
                         CHACHA20_TEST_AD_LENGTH,
                         CHACHA20_TEST_NONCE,
                         plaintext) != TAG_ERROR) { return ERROR; }
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_tag(void) {
    uint8_t plaintext[CHACHA20_TEST_CIPHER_LENGTH];
    uint8_t incorrect_tag[TAG_LENGTH];
    memcpy(incorrect_tag, CHACHA20_TEST_TAG, TAG_LENGTH);
    incorrect_tag[TAG_LENGTH / 2] ^= 1 << 4;
    if (chacha20_decrypt(CHACHA20_TEST_KEY,
                         CHACHA20_TEST_ENCRYPTED_CIPHER,
                         CHACHA20_TEST_CIPHER_LENGTH,
                         incorrect_tag,
                         CHACHA20_TEST_AD,
                         CHACHA20_TEST_AD_LENGTH,
                         CHACHA20_TEST_NONCE,
                         plaintext) != TAG_ERROR) { return ERROR; }
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_cipher_size(void) {
    uint8_t plaintext[CHACHA20_TEST_CIPHER_LENGTH];
    uint8_t wrong_length_cipher[CHACHA20_TEST_CIPHER_LENGTH / 2];
    memcpy(wrong_length_cipher, CHACHA20_TEST_ENCRYPTED_CIPHER, CHACHA20_TEST_CIPHER_LENGTH / 2);
    if (chacha20_decrypt(CHACHA20_TEST_KEY,
                         wrong_length_cipher,
                         CHACHA20_TEST_CIPHER_LENGTH,
                         CHACHA20_TEST_TAG,
                         CHACHA20_TEST_AD,
                         CHACHA20_TEST_AD_LENGTH,
                         CHACHA20_TEST_NONCE,
                         plaintext) == SUCCESS) { return ERROR; }
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_cipher_length(void) {
    uint8_t plaintext[CHACHA20_TEST_CIPHER_LENGTH];
    if (chacha20_decrypt(CHACHA20_TEST_KEY,
                         CHACHA20_TEST_ENCRYPTED_CIPHER,
                         CHACHA20_TEST_CIPHER_LENGTH / 2,
                         CHACHA20_TEST_TAG,
                         CHACHA20_TEST_AD,
                         CHACHA20_TEST_AD_LENGTH,
                         CHACHA20_TEST_NONCE,
                         plaintext) == SUCCESS) { return ERROR; }
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_ad_size(void) {
    uint8_t plaintext[CHACHA20_TEST_CIPHER_LENGTH];
    uint8_t wrong_length_ad[CHACHA20_TEST_AD_LENGTH / 2];
    memcpy(wrong_length_ad, CHACHA20_TEST_AD, CHACHA20_TEST_AD_LENGTH / 2);
    if (chacha20_decrypt(CHACHA20_TEST_KEY,
                         CHACHA20_TEST_ENCRYPTED_CIPHER,
                         CHACHA20_TEST_CIPHER_LENGTH,
                         CHACHA20_TEST_TAG,
                         wrong_length_ad,
                         CHACHA20_TEST_AD_LENGTH,
                         CHACHA20_TEST_NONCE,
                         plaintext) == SUCCESS) { return ERROR; }
    return SUCCESS;
}

static int chacha20_test_decrypt_incorrect_ad_length(void) {
    uint8_t plaintext[CHACHA20_TEST_CIPHER_LENGTH];
    if (chacha20_decrypt(CHACHA20_TEST_KEY,
                         CHACHA20_TEST_ENCRYPTED_CIPHER,
                         CHACHA20_TEST_CIPHER_LENGTH / 2,
                         CHACHA20_TEST_TAG,
                         CHACHA20_TEST_AD,
                         CHACHA20_TEST_AD_LENGTH,
                         CHACHA20_TEST_NONCE,
                         plaintext) == SUCCESS) { return ERROR; }
    return SUCCESS;
}

#endif
