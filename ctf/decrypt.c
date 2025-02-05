#include <stdbool.h>

#include "decrypt.h"
#include "common.h"
#include "hash.h"

#define NUM_KEYS 256
#define NUM_CHANNELS 5

// initial values of lib pointers
// add a blank (defualt) key with the root for channel 0
key_entry lib[NUM_KEYS] = {{.key = {}, .start = 0, .level = 64, .valid = true}};
key_entry* lib_chan[NUM_CHANNELS + 1] = {lib, lib+1, lib+1, lib+1, lib+1, lib+1};

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
inline bool get_subscription(frame_number frame, channel_number chan, key_entry* key);

int derive_key(frame_number frame, channel_number chan, uint8_t key[32]) {

}

inline key_entry hash_towards_frame(key_entry entry, frame_number frame) {

}

inline bool key_contains_frame(key_entry entry, frame_number frame) {

}

void add_subscription(struct subscription* sub) {

}

inline bool get_subscription(frame_number frame, channel_number chan, key_entry* key) {

}