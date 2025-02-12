#ifndef CRYPTO_TESTS_H
#define CRYPTO_TESTS_H
#include "simple_flash.h"
#include "host_messaging.h"
#include "simple_uart.h"

#include "crypto.h"
/**
 * GROUND TRUTHS
 * Nonce: 041caf4b50d0e4abf1e7bfcd
 * Key: 
 */
const uint8_t CHACHA20_TEST_KEY[] = { 0x41, 0x89, 0x06, 0x5b, 0x77, 0xd5, 0x18, 0x4d, 0x81, 0xc8, 0xce, 0x38, 0x10, 0xca, 0x35, 0x8d, 
                                      0x56, 0x51, 0x44, 0x32, 0xc1, 0x67, 0x2f, 0x2f, 0x2d, 0xd0, 0xb1, 0x0b, 0x4d, 0x14, 0x7f, 0xb5 };
const uint8_t CHACHA20_TEST_NONCE[] = { 0x04, 0x1c, 0xaf, 0x4b, 0x50, 0xd0, 0xe4, 0xab, 0xf1, 0xe7, 0xbf, 0xcd };

static int decrypt_frame_test_normal(void);

static int decrypt_frame_test_normal(void) {

    decrypt_frame(const uint8_t key[CRYPTO_KEY_SIZE],
                  const uint8_t magic[MAGIC_SIZE],
                  uint8_t channel_no,
                  uint64_t frame_no,
                  const uint8_t nonce[NONCE_SIZE],
                  const uint8_t encrypted_frame[CRYPTO_FRAME_SIZE],
                  const uint8_t tag[TAG_LENGTH],
                  uint8_t* out);
    return SUCCESS;
}

#endif
