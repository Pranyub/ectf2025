#include "cc20p1305.h"

int chacha20_decrypt(const uint8_t key[KEY_SIZE],
                     const uint8_t* in,
                     size_t len,
                     const uint8_t tag[TAG_LENGTH],
                     const uint8_t* ad,
                     size_t ad_len,
                     const uint8_t nonce[NONCE_SIZE],
                     uint8_t* out) {
    uint8_t encrypted_buffer[len + TAG_LENGTH];
    memcpy(encrypted_buffer, in, len);
    memcpy(encrypted_buffer + len, tag, TAG_LENGTH);

    size_t decryption_size = portable_chacha20_poly1305_decrypt(out,
                                                                key,
                                                                nonce,
                                                                ad,
                                                                ad_len,
                                                                encrypted_buffer,
                                                                len + TAG_LENGTH);
    

    if (decryption_size == -1) return ERROR;

    return SUCCESS;
}