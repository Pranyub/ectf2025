#include "cc20p1305.h"

int chacha20_decrypt(const uint8_t key[32],
                     const uint8_t* in,
                     size_t len,
                     const uint8_t tag[16],
                     const uint8_t* ad,
                     size_t ad_len,
                     uint8_t* out) {
    return SUCCESS;
}