#ifndef CRYPTO_H
#define CRYPTO_H

#include <string.h>

// debug
#include "simple_flash.h"
#include "host_messaging.h"
#include "simple_uart.h"

// required wolfssl includes
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

// crypto related imports
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/chacha20_poly1305.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/hmac.h>

#include "status_led.h"
#include "constants.h"
#include "decoder.h"
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


/**
 * @brief Decrypts a chacha20 encrypted cipher with a key and checks the tag
 * @pre \p key is exactly 32 bytes
 * @pre \p in is at least \p len bytes
 * @pre \p out is a buffer of at least \p len bytes
 * @pre \p ad is at least \p ad_len bytes
 * @pre \p nonce is exactly 12 bytes
 * @post \p out now stores the plaintext
 * @param key the key to be used for decryption
 * @param in a pointer to the input (encrypted) cipher
 * @param len the length of the \p in buffer, and the \p out buffer
 * @param tag the tag to be checked (for authentication)
 * @param ad the additional data to be checked for authenticity
 * @param ad_len the length of \p ad in bytes
 * @param nonce the number used once for chacha
 * @param out the output buffer that should store the decrypted cipher
 * @return SUCCESS on success, ERROR on general error, TAG_ERROR on tag mismatch
 */
int chacha20_decrypt(const uint8_t key[CHACHA_KEY_SIZE],
                     const uint8_t* in,
                     size_t len,
                     const uint8_t tag[TAG_LENGTH],
                     const uint8_t* ad,
                     size_t ad_len,
                     const uint8_t nonce[NONCE_SIZE],
                     uint8_t* out);

/**
 * @brief decrypts a frame with authentication checking, given an additional data that's already in a uint8_t array
 * @pre \p out is at least the same size as \p buf
 * @pre \p tag is a valid tag for the frame
 * @pre \p additional_data is the formatted additional data in the following order:
 *                 1 byte      8 bytes           12 bytes
 *                ---------------------------------------------
 *                |       |               |                   |
 *                | CH. # |    FRAME #    |       NONCE       |
 *                |       |               |                   |
 *                ---------------------------------------------
 * @post \p out now stores the decrypted frame
 * @param key the key used for decryption
 * @param additional_data the additional data to be used authentication
 * @param encrypted_frame the input buffer with the encrypted frame
 * @param tag the tag used for authentication checking
 * @param out the output buffer that will store the decrypted frame
 * @return SUCCESS on success, and ERROR on general error, and TAG_ERROR on tag mismatch
 */
int decrypt_frame_ad_buf(const uint8_t key[CHACHA_KEY_SIZE],
                         const uint8_t additional_data[sizeof(uint8_t) + sizeof(uint64_t) + NONCE_SIZE],
                         const uint8_t encrypted_frame[CRYPTO_FRAME_SIZE],
                         const uint8_t tag[TAG_LENGTH],
                         uint8_t* out);

/**
 * @brief decrypts a frame that is encoded as a raw byte array
 * @pre \p out is at least the same size as the ciphertext
 * @post \p out now stores the decrypted frame
 * @param key the key used for decryption
 * @param in the input frame which includes the AD, encrypted frame, and tag formatted as such
 *     -------------------- AD --------------------
 *      1 byte      8 bytes           12 bytes                 64 bytes             16 bytes
 *     -----------------------------------------------------------------------------------------
 *     |       |               |                   |                            |              |
 *     | CH. # |    FRAME #    |       NONCE       |        Encrypted Frame     |   Auth Tag   |
 *     |       |               |                   |                            |              |
 *     -----------------------------------------------------------------------------------------
 * @param out the output buffer that will store the decrypted frame
 * @return SUCCESS on success, and ERROR on general error, and TAG_ERROR on tag mismatch
 */
int decrypt_frame_raw(const uint8_t key[CHACHA_KEY_SIZE],
                      const uint8_t in[sizeof(uint8_t) +
                                       sizeof(uint64_t) +
                                       NONCE_SIZE +
                                       CRYPTO_FRAME_SIZE +
                                       TAG_LENGTH],
                      uint8_t* out);

/**
 * @brief decrypts a frame with authentication checking
 * @pre \p out is at least the same size as \p buf
 * @pre \p tag is a valid tag for the frame
 * @post \p out now stores the decrypted frame
 * @param key the key used for decryption
 * @param channel_no the channel number of the frame
 * @param frame_no the frame number
 * @param nonce the number used once
 * @param encrypted_frame the input buffer with the encrypted frame
 * @param tag the tag used for authentication checking
 * @param out the output buffer that will store the decrypted frame
 * @return SUCCESS on success, and ERROR on general error, and TAG_ERROR on tag mismatch
 */
int decrypt_frame(const uint8_t key[CHACHA_KEY_SIZE],
                  uint8_t channel_no,
                  uint64_t frame_no,
                  const uint8_t nonce[NONCE_SIZE],
                  const uint8_t encrypted_frame[CRYPTO_FRAME_SIZE],
                  const uint8_t tag[TAG_LENGTH],
                  uint8_t* out);

/**
 * @brief decrypts a subscription using the KDK and the device ID
 * @pre \p update is a valid structured update packet
 * @post the subscription is now stored in \p sub
 * @return SUCCESS on success, ERROR on general error, TAG_ERROR on tag mismatch
 */
int decrypt_subscription_update(const uint8_t kdk[CHACHA_KEY_SIZE],
                                const subscription_update_packet_t* update,
                                subscription_t* sub);

/**
 * @brief verifies the ECDSA signature 
 * @see https://github.com/wolfSSL/wolfssl-examples/blob/master/ecc/
 * @note To understand the verify function: https://www.wolfssl.com/doxygen/group__ECC.html#ga5b1bb1c6ce3f9238c8f23a3e516952bb
 * @param in the signed data to be verified
 * @param len the length of \p in
 * @param sig the signature to be checked against \p data
 * @param pub the public key to be used
 * @return SUCCESS on success, ERROR on error, ECDSA_VERIFY_ERROR on signature verification fail
 */
int ecdsa_verify(const uint8_t* in,
                 size_t len,
                 const uint8_t sig[ECDSA_SIGNATURE_SIZE],
                 const uint8_t pub[CRYPTO_ECC_KEY_SIZE]);

/**
 * @brief verifies the ECDSA signature given the SHA256 hash and signature
 * @param hash the hash to be verfiied
 * @param sig the signature to be checked against \p data
 * @param pub the public key to be used
 * @return SUCCESS on success, ERROR on error, ECDSA_VERIFY_ERROR on signature verification fail
 */
int ecdsa_verify_sha256hash(const uint8_t hash[SHA256_DIGEST_SIZE],
                            const uint8_t sig[ECDSA_SIGNATURE_SIZE],
                            const uint8_t pub[CRYPTO_ECC_KEY_SIZE]);

/**
 * @brief derives a key using HKDF and SHA-256
 * @pre \p out is at least 32 bytes long.
 * @post \p out now stores the derived key.
 * @param kdk the input key to derive other keys from.
 * @param info the additional information to be used to derived the key (e.g., decoder_id).
 * @param info_length the length of \p info in bytes.
 * @param salt the salt to be used.  Probably going to be a nonce of some kind.
 * @param salt_length the length of the \p salt in bytes.
 * @param out a pointer to where the derived key should be stored.
 */
int hkdf_derive_key(const uint8_t kdk[CHACHA_KEY_SIZE],
                    const uint8_t* info,
                    size_t info_length,
                    const uint8_t* salt,
                    uint8_t salt_length,
                    uint8_t* out);

#endif
