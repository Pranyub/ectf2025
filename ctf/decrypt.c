#include <stdbool.h>

#include "decrypt.h"
#include "common.h"
#include "hash.h"

/**
 * Returns true iff the key for the given frame can be derived from the given key
 */
inline bool key_contains_frame(key_entry entry, frame_number frame);

/**
 * Hashes the key to get the next key
 * Successive calls will eventually reach the key for the given frame
 */
inline key_entry hash_towards_frame(key_entry entry, frame_number frame);

/**
 * Gets the correct key for the given frame from the library
 * Returns SUCCESS on success, else FAILURE
 */
inline bool get_subscription(frame_number frame, key_entry* key);

int derive_key(uint64_t packet_id, uint32_t channel_no, uint8_t key[32]) {

}

inline key_entry hash_towards_frame(key_entry entry, frame_number frame) {

}

inline bool key_contains_frame(key_entry entry, frame_number frame) {
	
}

void add_subscription(struct subscription* sub) {

}

inline bool get_subscription(frame_number frame, key_entry* key) {

}