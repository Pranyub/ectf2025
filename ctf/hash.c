#include <stdio.h>
#include <openssl/sha.h>

#include "hash.h"

/**
 * Hashes all keys under this key into dst
 */
void hash_key(key_entry key ,uint64_t frame_num, key_entry* dst) {
	SHA256_CTX sha256;
	
    SHA256_Init(&sha256);

    SHA256_Update(&sha256, key.key, sizeof(key));
	SHA256_Update(&sha256, frame_num, sizeof(frame_num));
    
    // Finalize and store the result in output
    SHA256_Final(dst, &sha256);

}