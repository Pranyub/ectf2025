#include "crypto.h"

int chacha20_decrypt(const uint8_t key[KEY_SIZE],
                     const uint8_t* in,
                     size_t len,
                     const uint8_t tag[TAG_LENGTH],
                     const uint8_t* ad,
                     size_t ad_len,
                     const uint8_t nonce[NONCE_SIZE],
                     uint8_t* out) {
    int status = wc_ChaCha20Poly1305_Decrypt(key, nonce, ad, ad_len, in, len, tag, out);
    if (status == 0) {
        return SUCCESS;
    }
    if (status == BAD_FUNC_ARG) {
        return ERROR;
    }

    if (status == MAC_CMP_FAILED_E) {
        return TAG_ERROR;
    }

    return ERROR;  // generic error
}
