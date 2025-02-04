#pragma once

#include <types.h>

typedef uint64_t frame_number;

typedef struct key_entry {
	// the key itself
	uint8_t key[64];
	
	// the first frame number covered by this key
	frame_number start;

	// the number of nodes below this
	// len == 1 << level
	uint8_t level;
} key_entry;

#define SUBSCRIPTION_NUM_BLOCKS 30
typedef struct subscription {
	frame_number start;
	frame_number len;
	key_entry keys[SUBSCRIPTION_NUM_BLOCKS];
	uint8_t nkeys;
} subscription;