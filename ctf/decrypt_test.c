// shhh you saw nothing
#include "decrypt.c"

#include "common.h"

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
	assert(lib_chan[1] + 3 == lib_chan[2]);
	assert(lib_chan[2] == lib_chan[3]);
	assert(lib_chan[2] == lib_chan[4]);
	assert(lib_chan[2] == lib_chan[5]);
	
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

/**
 * Testbench for decrypt functions
 */
int main(int argc, char** argv) {
	test_addSubscription();
}