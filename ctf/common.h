#pragma once

#include <types.h>

typedef unsigned long long int key;
typedef unsigned long long int frame_number;
typedef unsigned int salt;

typedef struct key_entry {
	key key;
	frame_number start;
	frame_number len;
} key_entry;

#define SUBSCRIPTION_NUM_BLOCKS 30
typedef struct subscription {
	frame_number start;
	frame_number len;
	key_entry keys[SUBSCRIPTION_NUM_BLOCKS];
	uint8_t nkeys;
} subscription;

// block of keys for 256 frames
#define KEY_BLOCK_LEN 256
#define KEY_FRAME_NUM_BITS 8 // 2^KEY_FRAME_NUM_BITS = KEY_BLOCK_LEN
#define FRAME_BITS(x) ((x) & 0xFF)
#define BLOCK_BITS(x) ((x) & ~0xFF)

typedef struct key_block {
	frame_number block;
	key keys[KEY_BLOCK_LEN];
} key_block;
