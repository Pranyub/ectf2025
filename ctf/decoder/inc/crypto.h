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

#define TAG_LENGTH 16
#define CHACHA_KEY_SIZE 32
#define NONCE_SIZE 12
#define CRYPTO_FRAME_SIZE 256
#define MAGIC_SIZE 3

#define ECDSA_SIGNATURE_SIZE 64
#define CRYPTO_ECC_KEY_SIZE 65
#define ECC_CURVE ECC_SECP256K1

#define SUCCESS 0
#define ERROR 1
#define TAG_ERROR 2
#define ECDSA_VERIFY_ERROR 3

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
 *                                   3 bytes   1 byte      8 bytes           12 bytes
 *                                ---------------------------------------------------------
 *                                |           |       |               |                   |
 *                                |   MAGIC   | CH. # |    FRAME #    |       NONCE       |
 *                                |           |       |               |                   |
 *                                ---------------------------------------------------------
 * @post \p out now stores the decrypted frame
 * @param key the key used for decryption
 * @param additional_data the additional data to be used authentication
 * @param encrypted_frame the input buffer with the encrypted frame
 * @param tag the tag used for authentication checking
 * @param out the output buffer that will store the decrypted frame
 * @return SUCCESS on success, and ERROR on general error, and TAG_ERROR on tag mismatch
 */
int decrypt_frame_ad_buf(const uint8_t key[CHACHA_KEY_SIZE],
                         const uint8_t additional_data[MAGIC_SIZE + sizeof(uint8_t) + sizeof(uint64_t) + NONCE_SIZE],
                         const uint8_t encrypted_frame[CRYPTO_FRAME_SIZE],
                         const uint8_t tag[TAG_LENGTH],
                         uint8_t* out);

/**
 * @brief decrypts a frame that is encoded as a raw byte array
 * @pre \p out is at least the same size as the ciphertext
 * @post \p out now stores the decrypted frame
 * @param key the key used for decryption
 * @param in the input frame which includes the AD, encrypted frame, and tag formatted as such
 *      ------------------------- AD --------------------------
 *        3 bytes   1 byte      8 bytes           12 bytes                256 bytes             16 bytes
 *     -----------------------------------------------------------------------------------------------------
 *     |           |       |               |                   |                            |              |
 *     |   MAGIC   | CH. # |    FRAME #    |       NONCE       |        Encrypted Frame     |   Auth Tag   |
 *     |           |       |               |                   |                            |              |
 *     -----------------------------------------------------------------------------------------------------
 * @param out the output buffer that will store the decrypted frame
 * @return SUCCESS on success, and ERROR on general error, and TAG_ERROR on tag mismatch
 */
int decrypt_frame_raw(const uint8_t key[CHACHA_KEY_SIZE],
                      const uint8_t in[MAGIC_SIZE +
                                       sizeof(uint8_t) +
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
 * @brief verifies the ECDSA signature 
 * @see https://github.com/wolfSSL/wolfssl-examples/blob/master/ecc/
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

#endif
