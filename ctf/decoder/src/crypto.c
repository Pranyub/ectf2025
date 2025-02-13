#include "crypto.h"

int chacha20_decrypt(const uint8_t key[CHACHA_KEY_SIZE],
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

int decrypt_frame_ad_buf(const uint8_t key[CHACHA_KEY_SIZE],
                         const uint8_t additional_data[MAGIC_SIZE + sizeof(uint8_t) + sizeof(uint64_t) + NONCE_SIZE],
                         const uint8_t encrypted_frame[CRYPTO_FRAME_SIZE],
                         const uint8_t tag[TAG_LENGTH],
                         uint8_t* out) {
    size_t ad_size = MAGIC_SIZE + sizeof(uint8_t) + sizeof(uint64_t) + NONCE_SIZE;
    return chacha20_decrypt(key,
                            encrypted_frame,
                            CRYPTO_FRAME_SIZE,
                            tag,
                            additional_data,
                            ad_size,
                            additional_data + ad_size - NONCE_SIZE,
                            out);
    
}

int decrypt_frame_raw(const uint8_t key[CHACHA_KEY_SIZE],
                      const uint8_t in[MAGIC_SIZE +
                                       sizeof(uint8_t) +
                                       sizeof(uint64_t) +
                                       NONCE_SIZE +
                                       CRYPTO_FRAME_SIZE +
                                       TAG_LENGTH],
                      uint8_t* out) {
    size_t ad_size = MAGIC_SIZE + sizeof(uint8_t) + sizeof(uint64_t) + NONCE_SIZE;
    return chacha20_decrypt(key,
                            in + ad_size,
                            CRYPTO_FRAME_SIZE,
                            in + ad_size + CRYPTO_FRAME_SIZE,
                            in,
                            ad_size,
                            in + ad_size - NONCE_SIZE,
                            out);
    // return decrypt_frame_ad_buf(key, in, in + ad_size, in + ad_size + CRYPTO_GRAME_SIZE);
}

int decrypt_frame(const uint8_t key[CHACHA_KEY_SIZE],
                  const uint8_t magic[MAGIC_SIZE],
                  uint8_t channel_no,
                  uint64_t frame_no,
                  const uint8_t nonce[NONCE_SIZE],
                  const uint8_t encrypted_frame[CRYPTO_FRAME_SIZE],
                  const uint8_t tag[TAG_LENGTH],
                  uint8_t* out) {
    int offset = 0;

    // allocate buffer for additional data
    size_t ad_size = MAGIC_SIZE + sizeof(channel_no) + sizeof(frame_no) + NONCE_SIZE;
    uint8_t additional_data[ad_size];

    memcpy((void*)(additional_data + offset), (void*)magic, MAGIC_SIZE);  // copy magic to AD
    offset += MAGIC_SIZE;

    memcpy((void*)(additional_data + offset), (void*)(&channel_no), sizeof(channel_no));  // copy the channel number to the AD
    offset += sizeof(channel_no);

    memcpy((void*)(additional_data + offset), (void*)(&frame_no), sizeof(frame_no));  // copy the frame number to the AD
    offset += sizeof(frame_no);

    memcpy((void*)(additional_data + offset), (void*)nonce, NONCE_SIZE);  // copy the nonce to the AD
    return decrypt_frame_ad_buf(key, additional_data, encrypted_frame, tag, out);
}