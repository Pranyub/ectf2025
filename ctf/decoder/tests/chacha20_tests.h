#ifndef CHACHA20_TEST_H
#define CHACHA20_TEST_H

#include <stdio.h>
#include <string.h>

#include "simple_flash.h"
#include "host_messaging.h"
#include "simple_uart.h"

#include "crypto.h"

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
