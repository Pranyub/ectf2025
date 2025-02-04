#include <stdio.h>
#include <openssl/sha.h>
#include "hash.h"

#define PAD_LENGTH 256

/**
 * Hashes all keys under this key into dst
 */
void hash_key(key_entry key ,uint64_t frame_num, key_entry* dst) {
	SHA256_CTX sha256;
    int padding_len = sizeof(key.key) + sizeof(frame_num);
	
    SHA256_Init(&sha256);

    SHA256_Update(&sha256, key.key, sizeof(key));
	SHA256_Update(&sha256, frame_num, sizeof(frame_num));
	
    //fix padding so that it's same with Taylor
    if (padding_len < PAD_LENGTH) {
        char padding[PAD_LENGTH - padding_len];
        memset(padding, 0, sizeof(padding));
        SHA256_Update(&sha256, padding, sizeof(padding));
    }
    
    // Finalize and store the result in output
    SHA256_Final(dst, &sha256);

}