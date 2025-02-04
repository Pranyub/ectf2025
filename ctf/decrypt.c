#include <stdbool.h>

#include "decrypt.h"
#include "common.h"
#include "hash.h"

// array for key blocks
// sorted in increasing order by key_entry.start
// should not contain any duplicate or redundant keys
#define MAX_BLOCKS 128
static key_entry keys[MAX_BLOCKS];
uint16_t firstkey = 0;
uint16_t keylen = 0;

/**
 * Returns true iff some part of this key is inside the block
 */
inline bool block_uses_key(frame_number block, key_entry key) {
	return (block >> KEY_FRAME_NUM_BITS) >= (key.start >> KEY_FRAME_NUM_BITS) && 
		(block >> KEY_FRAME_NUM_BITS) < (key.start >> KEY_FRAME_NUM_BITS + key.len >> KEY_FRAME_NUM_BITS);
}

/**
 * Returns true iff the outer key contains all frame of the inner key within it
 */
inline bool key_contains_key(key_entry outer, key_entry inner) {
	// todo
}

/**
 * Hashes all keys under this key into dst
 */
void hash_key(key_block* dst, key_entry key, frame_number block) {
	if (key.len == 1) {
		dst->keys[FRAME_BITS(key.start)] = key.key;
		return;
	}
	key_entry newkey;
	newkey.len = key.len / 2;
	newkey.start = key.start;
	if (block_uses_key(block, newkey)) {
		newkey.key = hash(key.key, newkey.start, newkey.len);
		hash_key(dst, newkey, block);
	}
	newkey.start += newkey.len;
	if (block_uses_key(block, newkey)) {
		newkey.key = hash(key.key, newkey.start, newkey.len);
		hash_key(dst, newkey, block);
	}
}

void hash_block(key_block* dst, frame_number block) {
	for (int i = firstkey; i < firstkey + keylen; i++) {
		// if needed, use this key as much as possible
		if (block_uses_key(block, keys[i])) hash_key(dst, keys[i], block);
		// return if this key can be used for the next block
		if (block_uses_key(block + KEY_BLOCK_LEN, keys[i])) return;
	}
}

void add_subscription(subscription* sub) {
	// find the first element such that an element from sub is a better choice
	int libstart = firstkey;
	int substart = 0;
	while (true) {
		if (substart == sub->len) {
			// entire subscription redundant
			return;
		}
		if (key_contains_key(keys[libstart], sub->keys[substart])) {
			substart++;
		} else if (key_contains_key(sub->keys[substart], keys[libstart])) {
			libstart++;
			if (libstart == keylen) break;
		} else if (keys[libstart].start < sub->keys[substart].start) {
			break;
		}
	}

	// find the first element such that an element from lib is a better choice
	int libend = firstkey + len;
	int subend = sub->len;


	
}