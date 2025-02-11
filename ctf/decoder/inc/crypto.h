#ifndef CRYPTO_H
#define CRYPTO_H

#include <string.h>
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/chacha20_poly1305.h>

#define TAG_LENGTH 16
#define KEY_SIZE 32
#define NONCE_SIZE 12
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
int chacha20_decrypt(const uint8_t key[KEY_SIZE],
                     const uint8_t* in,
                     size_t len,
                     const uint8_t tag[TAG_LENGTH],
                     const uint8_t* ad,
                     size_t ad_len,
                     const uint8_t nonce[NONCE_SIZE],
                     uint8_t* out);

#endif
