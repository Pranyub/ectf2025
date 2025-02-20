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
                         const uint8_t additional_data[sizeof(uint8_t) + sizeof(uint64_t) + NONCE_SIZE],
                         const uint8_t encrypted_frame[CRYPTO_FRAME_SIZE],
                         const uint8_t tag[TAG_LENGTH],
                         uint8_t* out) {
    size_t ad_size = sizeof(uint8_t) + sizeof(uint64_t) + NONCE_SIZE;
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
                      const uint8_t in[sizeof(uint8_t) +
                                       sizeof(uint64_t) +
                                       NONCE_SIZE +
                                       CRYPTO_FRAME_SIZE +
                                       TAG_LENGTH],
                      uint8_t* out) {
    size_t ad_size = sizeof(uint8_t) + sizeof(uint64_t) + NONCE_SIZE;
    return chacha20_decrypt(key,
                            in + ad_size,
                            CRYPTO_FRAME_SIZE,
                            in + ad_size + CRYPTO_FRAME_SIZE,
                            in,
                            ad_size,
                            in + ad_size - NONCE_SIZE,
                            out);
}

int decrypt_frame(const uint8_t key[CHACHA_KEY_SIZE],
                  uint8_t channel_no,
                  uint64_t frame_no,
                  const uint8_t nonce[NONCE_SIZE],
                  const uint8_t encrypted_frame[CRYPTO_FRAME_SIZE],
                  const uint8_t tag[TAG_LENGTH],
                  uint8_t* out) {
    int offset = 0;

    // allocate buffer for additional data
    size_t ad_size = sizeof(channel_no) + sizeof(frame_no) + NONCE_SIZE;
    uint8_t additional_data[ad_size];

    memcpy((void*)(additional_data + offset), (void*)(&channel_no), sizeof(channel_no));  // copy the channel number to the AD
    offset += sizeof(channel_no);

    memcpy((void*)(additional_data + offset), (void*)(&frame_no), sizeof(frame_no));  // copy the frame number to the AD
    offset += sizeof(frame_no);

    memcpy((void*)(additional_data + offset), (void*)nonce, NONCE_SIZE);  // copy the nonce to the AD
    return decrypt_frame_ad_buf(key, additional_data, encrypted_frame, tag, out);
}

int ecdsa_verify(const uint8_t* in,
                 size_t len,
                 const uint8_t sig[ECDSA_SIGNATURE_SIZE],
                 const uint8_t pub[CRYPTO_ECC_KEY_SIZE]) {
    ecc_key eccKey;
    mp_int r, s;
    if (wc_ecc_init(&eccKey) < 0) {
        return ERROR;
    }

    if (mp_init(&r) != MP_OKAY) {
        wc_ecc_free(&eccKey);
        return ERROR;
    }
    if (mp_init(&s) != MP_OKAY) {
        mp_clear(&r);
        wc_ecc_free(&eccKey);
        return ERROR;
    }

    if (wc_ecc_import_unsigned(&eccKey, pub + 1, pub + 1 + CRYPTO_ECC_KEY_SIZE / 2, NULL, ECC_CURVE) != 0) {
        mp_clear(&r);
        mp_clear(&s);
        wc_ecc_free(&eccKey);
        return ERROR;
    }

    if (eccKey.type != ECC_PUBLICKEY) {
        mp_clear(&r);
        mp_clear(&s);
        wc_ecc_free(&eccKey);
        return ERROR;
    }

    mp_read_unsigned_bin(&r, sig, ECDSA_SIGNATURE_SIZE / 2);
    mp_read_unsigned_bin(&s, sig + ECDSA_SIGNATURE_SIZE / 2, ECDSA_SIGNATURE_SIZE / 2);
    
    uint8_t hash[SHA256_DIGEST_SIZE];
    int res = 0;
    if (wc_Sha256Hash(in, len, hash) != 0 ||
        wc_ecc_verify_hash_ex(&r, &s, hash, SHA256_DIGEST_SIZE, &res, &eccKey) != MP_OKAY) {
        mp_clear(&r);
        mp_clear(&s);
        wc_ecc_free(&eccKey);
        return ERROR;
    }

    mp_clear(&r);
    mp_clear(&s);
    wc_ecc_free(&eccKey);
    if (res != 1) {
        return ECDSA_VERIFY_ERROR;
    }
    return SUCCESS;
}

int ecdsa_verify_sha256hash(const uint8_t hash[SHA256_DIGEST_SIZE],
                            const uint8_t sig[ECDSA_SIGNATURE_SIZE],
                            const uint8_t pub[CRYPTO_ECC_KEY_SIZE]) {
    ecc_key eccKey;
    mp_int r, s;
    if (wc_ecc_init(&eccKey) < 0) {
        return ERROR;
    }

    if (mp_init(&r) != MP_OKAY) {
        wc_ecc_free(&eccKey);
        return ERROR;
    }
    if (mp_init(&s) != MP_OKAY) {
        mp_clear(&r);
        wc_ecc_free(&eccKey);
        return ERROR;
    }

    if (wc_ecc_import_unsigned(&eccKey, pub + 1, pub + 1 + CRYPTO_ECC_KEY_SIZE / 2, NULL, ECC_CURVE) != 0) {
        mp_clear(&r);
        mp_clear(&s);
        wc_ecc_free(&eccKey);
        return ERROR;
    }

    if (eccKey.type != ECC_PUBLICKEY) {
        mp_clear(&r);
        mp_clear(&s);
        wc_ecc_free(&eccKey);
        return ERROR;
    }

    mp_read_unsigned_bin(&r, sig, ECDSA_SIGNATURE_SIZE / 2);
    mp_read_unsigned_bin(&s, sig + ECDSA_SIGNATURE_SIZE / 2, ECDSA_SIGNATURE_SIZE / 2);
    
    int res = 0;
    if (wc_ecc_verify_hash_ex(&r, &s, hash, SHA256_DIGEST_SIZE, &res, &eccKey) != MP_OKAY) {
        mp_clear(&r);
        mp_clear(&s);
        wc_ecc_free(&eccKey);
        return ERROR;
    }

    mp_clear(&r);
    mp_clear(&s);
    wc_ecc_free(&eccKey);
    if (res != 1) {
        return ECDSA_VERIFY_ERROR;
    }
    return SUCCESS;
}

int verify_subscription(const subscription_update_packet_t* update, const uint8_t key[CRYPTO_ECC_KEY_SIZE]) {
    Sha256 sha256;
    uint8_t hash[SHA256_DIGEST_SIZE];
    if (wc_InitSha256(&sha256) != 0) {
        return ERROR;
    }

    wc_Sha256Update(&sha256, (const uint8_t*)(&(update->chan)), sizeof(update->chan));
    wc_Sha256Update(&sha256, (const uint8_t*)(&(update->nkeys)), sizeof(update->nkeys));
    wc_Sha256Update(&sha256, (const uint8_t*)(&(update->frame_start)), sizeof(update->frame_start));
    wc_Sha256Update(&sha256, (const uint8_t*)(&(update->frame_end)), sizeof(update->frame_end));
    for (int i = 0; i < update->nkeys; i++) {
        const key_entry_packet_t* key = &((update->keys)[i]);
        wc_Sha256Update(&sha256, (const uint8_t*)(&(key->frame_start)), sizeof(key->frame_start));
        wc_Sha256Update(&sha256, (const uint8_t*)(&(key->level)), sizeof(key->level));
        wc_Sha256Update(&sha256, (const uint8_t*)(&(key->key)), sizeof(key->key));
    }
    wc_Sha256Final(&sha256, hash);
    wc_Sha256Free(&sha256);

    return ecdsa_verify_sha256hash(hash, update->signature, key);
}
