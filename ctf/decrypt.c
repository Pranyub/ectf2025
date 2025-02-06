#include <stdbool.h>

#include "decrypt.h"
#include "common.h"
#include "hash.h"

#define NUM_KEYS 256
#define NUM_CHANNELS 5

/*
 * On the format and implementation of lib:
 * Lib is an array broken into separate sections for each channel
 * Each channel's section starts at its pointer in lib_chan (inclusive) and ends
 * at the next pointer in lib_chan (exclusive)
 * Thus, channel 0's keys are from lib_chan[0] to lib_chan[1]
 * 
 * Each section is sorted by frame number (increasing)
 * additionally, each section should have no redundant keys,
 * there should exist no frame that can be derived from two different keys in a section
 */

// initial values of lib pointers
// add a blank (defualt) key with the root for channel 0
key_entry lib[NUM_KEYS] = {{.key = {}, .start = 0, .level = 64, .valid = true}};

// pointers to the start of keys for each channel
key_entry* lib_chan[NUM_CHANNELS + 1] = {lib, lib+1, lib+1, lib+1, lib+1, lib+1};

#define LIB_END (lib + NUM_KEYS)

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

/**
 * Returns true iff the key outer fully contains inner
 * i.e., inner could be derived from outer
 */
inline bool key_contains_key(key_entry outer, key_entry inner);

int derive_key(frame_number frame, channel_number chan, uint8_t key[32]) {

}

inline key_entry hash_towards_frame(key_entry entry, frame_number frame) {

}

inline bool key_contains_frame(key_entry entry, frame_number frame) {

}

void add_subscription(struct subscription* sub) {
	// start lib ptr
	key_entry* slp = lib_chan[sub->chan];
	// end lib ptr
	key_entry* elp = lib_chan[sub->chan + 1];
	// start sub ptr
	key_entry* ssp = sub->keys;
	// end sub ptr
	key_entry* esp  = sub->keys + sub->nkeys;

	// assertions
	// slp <= elp
	// ssp < esp

	while (true) {
		// nothing useful in sub
		if (ssp >= esp) return;
		// nothing left in lib
		if (slp >= elp) break;

		if (slp->start < ssp->start) slp++;
		else if (key_contains_key(*slp, *ssp)) ssp++;
		else break;
	}

	// assertions
	// slp <= elp
	// ssp < esp
	// slp == elp || ssp.start < slp.start || ssp contains slp

	while (true) {
		// nothing useful in sub
		if (ssp >= esp) return;
		// nothing left in lib
		if (slp >= elp) break;
		
		if (elp[-1].start > esp[-1].start) elp--;
		else if (key_contains_key(elp[-1], esp[-1])) esp--;
		else break;
	}

	// assertions
	// slp <= elp
	// ssp < esp
	// slp == elp || ssp.start < slp.start || ssp contains slp
	// slp == elp || esp[-1].start < elp[-1].start || esp[-1] contains elp[-1]

	// offset to copy tail (everything past elp)
	int copy_dist = (esp - ssp) - (elp - slp);

	// copy tail to correct position
	if (copy_dist > 0) {
		if (lib_chan[NUM_CHANNELS + 1] + copy_dist > LIB_END) return;  // failure (out of space)
		for (key_entry* ptr = lib_chan[NUM_CHANNELS + 1] - 1; ptr >= elp; ptr--) {
			ptr[copy_dist] = ptr[0];
		}
	} else if (copy_dist < 0) {
		for (key_entry* ptr = elp; ptr < lib_chan[NUM_CHANNELS + 1]; ptr++) {
			ptr[copy_dist] = ptr[0];
		}
	}

	// copy sub into lib
	while (ssp < esp) {
		*slp++ = *ssp++;
	}

	// update offsets of lib_chan pointers
	for (int i = sub->chan + 1; i <= NUM_CHANNELS; i++) {
		lib_chan[i] += copy_dist;
	}
}

inline bool get_subscription(frame_number frame, channel_number chan, key_entry* key) {

}

inline bool key_contains_key(key_entry outer, key_entry inner) {

}