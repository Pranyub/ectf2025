#include <stdbool.h>
#include <string.h>

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/sha256.h>

#include "decrypt.h"
#include "common.h"

#define PAD_LENGTH 256

// add a blank (defualt) key with the root for channel 0
subscription lib[NUM_CHANNELS] = {{.start=0, .end=0, .chan=0, .nkeys=1 /*keys initialized to 0*/}};

/**
 * stk contains the current stack of derived keys
 * The highest level key is always at the bottom of the stack (stk[0])
 * derived keys are layered on top
 */
key_entry stk[NUM_CHANNELS][ROOT_KEY_LEVEL];
uint8_t stksize[NUM_CHANNELS];

/**
 * Returns true iff the key for the given frame can be derived from the given key
 */
inline bool key_contains_frame(key_entry entry, frame_number frame);

/**
 * Hashes the key to get the next key
 * Successive calls will eventually reach the key for the given frame
 */
inline key_entry hash_towards_frame(key_entry entry, frame_number frame, key_entry* dst);

/**
 * Hashes the source key with the start value of the dstkey to produce the value of the dstkey
 * outputs the dstkey into dstkey
 */
void hash_key(key_entry srckey, uint64_t dstkey_start, key_entry* dstkey);

int derive_key(frame_number frame, channel_number chan, uint8_t key[32]) {
	// check if key is found in stack
	if (stksize[chan] > 0 && key_contains_frame(stk[chan][0], frame)) {
		goto key_found;
	}

	// try to find the correct key from the library
	for (uint8_t i = 0; i < lib[chan].nkeys; i++) {
		if (key_contains_frame(lib[chan].keys[i], frame)) {
			stk[chan][0] = lib[chan].keys[i];
			stksize[chan] = 1;
			goto key_found;
		}
	}

	// didn't find correct key; fail
	return FAILURE;

	key_found:
	
	// pop the stack until the key on the top of the stack can be used to derive our frame
	while (!key_contains_frame(stk[chan][stksize[chan] - 1], frame)) stksize[chan]--;

	// derive keys from the top of the stack until we get the level 0 key
	while (stk[chan][stksize[chan] - 1].level) {
		hash_towards_frame(stk[chan][stksize[chan] - 1], frame, &stk[chan][stksize[chan]]);
		stksize[chan]++;
	}

	// copy the top key in stack out to the given dst array
	for (int i = 0; i < 32; i++) {
		key[i] = stk[chan][stksize[chan] - 1].key[i];
	}
	return SUCCESS;
}

inline key_entry hash_towards_frame(key_entry entry, frame_number frame, key_entry* dst) {
	// get the start value of the key we're looking for by bitmasking frame
	frame = (frame >> (entry.level - 1)) << (entry.level - 1);

	// call hash function
	hash_key(entry, frame, dst);
}

inline bool key_contains_frame(key_entry entry, frame_number frame) {
	return (entry.start >> entry.level) == (frame >> entry.level);
	// return !((entry.start ^ frame) >> entry.level);
}

void add_subscription(struct subscription* sub) {
	stksize[sub->chan] = 0;
	/*
	for (int i = 0; i < ROOT_KEY_LEVEL; i++) {
		stk[sub->chan][i] = (key_entry){};
	}
	for (int i = 0; i < SUBSCRIPTION_NUM_BLOCKS; i++) {
		lib[sub->chan].keys[i] = (key_entry){};
	}
	*/
	lib[sub->chan] = *sub;
}

void hash_key(key_entry key, uint64_t frame_num, key_entry* dst) {
	Sha256 sha256;
	wc_InitSha256(&sha256);

	char padding[PAD_LENGTH];
	memset(padding, 0, PAD_LENGTH);

	wc_Sha256Update(&sha256, &key.key, sizeof(key.key));
	wc_Sha256Update(&sha256, &frame_num, sizeof(frame_num));
	wc_Sha256Update(&sha256, padding, PAD_LENGTH - sizeof(frame_num));
    
    // Finalize and store the result in output
    wc_Sha256Final(&sha256, dst->key);
}