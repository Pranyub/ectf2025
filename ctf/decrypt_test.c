// shhh you saw nothing
#include "decrypt.c"

#include "common.h"

// maybe don't look into this either
#include "hash.h"

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>

key_entry keygen(frame_number start, frame_number len) {
	assert(__builtin_popcount(len) == 1);
	uint8_t level = 0;
	while (len != 1) {
		len >>= 1;
		level++;
	}
	return (key_entry){.level = level, .start = start};
}

frame_number tstk[ROOT_KEY_LEVEL];
uint8_t tstksize = 0;

// dummy implementation of hash_key for testing purposes
void hash_key(key_entry srckey, uint64_t dstkey_start, key_entry* dstkey) {
	assert(tstksize);
	
	// pop the next off the target stack
	frame_number target = tstk[--tstksize];
	assert(dstkey_start == target);

	dstkey->start = dstkey_start;
	dstkey->level = srckey.level - 1;

	// copy key for verification purposes
	for (int i = 0; i < 32; i++) {
		dstkey->key[i] = srckey.key[i];
	}
}

void check_lib_invariant() {
	// check that the channel pointers are sorted
	for (int i = 0; i < NUM_CHANNELS; i++) {
		assert(lib_chan[i] <= lib_chan[i+1]);
	}
	// check that each section contains only valid keys and is sorted
	for (int i = 0; i < NUM_CHANNELS; i++) {
		key_entry* stp;
		key_entry* etp = lib_chan[i+1];

		// validate every key is aligned
		for (stp = lib_chan[i]; stp < etp; stp++) {
			assert(stp->start % (1 << stp->level) == 0);
		}

		for (stp = lib_chan[i]; stp < etp - 1; stp++) {
			// validate key comes before the next key
			assert(stp[0].start < stp[1].start);

			// validate that the key does not contain the next key
			// inverse is impossible because it has a later start frame
			assert(!key_contains_key(stp[0], stp[1]));
		}
	}
}

void test_addSubscription() {
	printf("Start Test Add Subscription\n");
	
	// add a simple subscription
	subscription sub = {.chan = 1, .start=4, .end=7, .nkeys=2, .keys={keygen(4,2), keygen(6,1)}};
	add_subscription(&sub);
	check_lib_invariant();

	// append a small subscription
	sub = (subscription){.chan=1, .start=7, .end=8, .nkeys=1, .keys={keygen(7,1)}};
	add_subscription(&sub);
	check_lib_invariant();

	// prepend a small subscription
	sub = (subscription){.chan=1, .start=2, .end=3, .nkeys=1, .keys={keygen(2,1)}};
	add_subscription(&sub);
	check_lib_invariant();

	// full check of state of lib
	assert(lib[0].start == 0);
	assert(lib[0].level == 64);
	assert(lib[1].start == 2);
	assert(lib[1].level == 0);
	assert(lib[2].start == 4);
	assert(lib[2].level == 1);
	assert(lib[3].start == 6);
	assert(lib[3].level == 0);
	assert(lib[4].start == 7);
	assert(lib[4].level == 0);

	// full check of lib_chan
	assert(lib_chan[0] + 1 == lib_chan[1]);
	assert(lib_chan[1] + 4 == lib_chan[2]);
	assert(lib_chan[2] == lib_chan[3]);
	assert(lib_chan[2] == lib_chan[4]);
	assert(lib_chan[2] == lib_chan[5]);

	// replace a large portion at the start
	sub = (subscription){.chan=1, .start=0, .end=4, .nkeys=1, .keys={keygen(0,4)}};
	add_subscription(&sub);
	check_lib_invariant();
	assert(lib_chan[1]->start == 0);
	assert(lib_chan[1]->level == 2);

	// add a sub that doesn't have a useful start
	sub = (subscription){.chan=1, .start=1, .end=8, .nkeys=3, .keys={keygen(1,1), keygen(2,2), keygen(4,4)}};
	add_subscription(&sub);
	check_lib_invariant();

	// full check of lib
	assert(lib[0].start == 0);
	assert(lib[0].level == 64);
	assert(lib[1].start == 0);
	assert(lib[1].level == 2);
	assert(lib[2].start == 4);
	assert(lib[2].level == 2);

	// full check of lib_chan
	assert(lib_chan[0] + 1 == lib_chan[1]);
	assert(lib_chan[1] + 2 == lib_chan[2]);
	assert(lib_chan[2] == lib_chan[3]);
	assert(lib_chan[2] == lib_chan[4]);
	assert(lib_chan[2] == lib_chan[5]);

	// add a sub (setup for next test)
	sub = (subscription){.chan=1, .start=8, .end=12, .nkeys=1, .keys={keygen(8,4)}};
	add_subscription(&sub);
	check_lib_invariant();
	
	// add a sub that doesn't have a useful end
	sub = (subscription){.chan=1, .start=0, .end=10, .nkeys=2, .keys={keygen(0,8), keygen(8,2)}};
	add_subscription(&sub);
	check_lib_invariant();

	// full check of lib
	assert(lib[0].start == 0);
	assert(lib[0].level == 64);
	assert(lib[1].start == 0);
	assert(lib[1].level == 3);
	assert(lib[2].start == 8);
	assert(lib[2].level == 2);

	// full check of lib_chan
	assert(lib_chan[0] + 1 == lib_chan[1]);
	assert(lib_chan[1] + 2 == lib_chan[2]);
	assert(lib_chan[2] == lib_chan[3]);
	assert(lib_chan[2] == lib_chan[4]);
	assert(lib_chan[2] == lib_chan[5]);

	// reset lib_chan
	lib_chan[1] = lib_chan[2] = lib_chan[3] = lib_chan[4] = lib_chan[5] = lib_chan[0] + 1;

	printf("Finish Test Add Subscription\n");
}

void test_deriveKey() {
	printf("Start Test Derive Key\n");

	// add some things into lib
	lib[1] = keygen(0,4);
	lib[1].key[0] = 1;
	lib[2] = keygen(8,1);
	lib[2].key[0] = 2;
	lib[3] = keygen(10,2);
	lib[3].key[0] = 3;

	lib_chan[5] = lib_chan[4] = lib_chan[3] = lib_chan[2] = lib_chan[1] + 3;
	
	uint8_t key[32];

	// test frame 0, has key
	tstk[0] = 0;
	tstk[1] = 0;
	tstksize = 2;
	assert(derive_key(0, 1, key) == SUCCESS);
	assert(tstksize == 0);
	assert(key[0] == 1);

	// test frame 6, no key
	assert(derive_key(6, 1, key) == FAILURE);
	assert(tstksize == 0);

	// (implicit) test stack is retained

	// test frame 1, has key, stack partially known
	tstk[0] = 1;
	tstksize = 1;
	assert(derive_key(1, 1, key) == SUCCESS);
	assert(tstksize == 0);
	assert(key[0] == 1);

	// test frame 8, has key, clear stack
	assert(derive_key(8, 1, key) == SUCCESS);
	assert(tstksize == 0);
	assert(key[0] == 2);

	// test frame 11, has key, clear stack, non-start
	tstk[0] = 11;
	tstksize = 1;
	assert(derive_key(11, 1, key) == SUCCESS);
	assert(tstksize == 0);
	assert(key[0] == 3);
	

	



	printf("Finish Test Derive Key\n");
}

/**
 * Testbench for decrypt functions
 */
int main(int argc, char** argv) {
	test_addSubscription();
	test_deriveKey();
}