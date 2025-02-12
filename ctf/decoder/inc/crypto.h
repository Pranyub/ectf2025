#ifndef CRYPTO_H
#define CRYPTO_H

#include <string.h>

// required wolfssl includes
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/ssl.h>

// crypto related imports
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/chacha20_poly1305.h>
#include <wolfssl/wolfcrypt/ecc.h>

#define TAG_LENGTH 16
#define CHACHA_KEY_SIZE 32
#define NONCE_SIZE 12
#define CRYPTO_FRAME_SIZE 256
#define MAGIC_SIZE 3

#define SUCCESS 0
#define ERROR 1
#define TAG_ERROR 2

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
 * @brief decrypts a frame with authentication checking
 * @pre \p out is at least the same size as \p buf
 * @pre \p tag is a valid tag for the frame
 * @post \p out now stores the decrypted frame
 * @param key the key used for decryption
 * @param magic the magic used at the start of the frame
 * @param channel_no the channel number of the frame
 * @param frame_no the frame number
 * @param nonce the number used once
 * @param encrypted_frame the input buffer with the encrypted frame
 * @param tag the tag used for authentication checking
 * @param out the output buffer that will store the decrypted frame
 * @return SUCCESS on success, and ERROR on general error, and TAG_ERROR on tag mismatch
 */
int decrypt_frame(const uint8_t key[CHACHA_KEY_SIZE],
                  const uint8_t magic[MAGIC_SIZE],
                  uint8_t channel_no,
                  uint64_t frame_no,
                  const uint8_t nonce[NONCE_SIZE],
                  const uint8_t encrypted_frame[CRYPTO_FRAME_SIZE],
                  const uint8_t tag[TAG_LENGTH],
                  uint8_t* out);

/**
 * @brief verifies the ECDSA signature of a hash
 * @see https://github.com/wolfSSL/wolfssl-examples/blob/master/ecc/ecc-params.c
 */

#endif
