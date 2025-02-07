#include <stdbool.h>

#include "decrypt.h"
#include "common.h"
#include "hash.h"

#define NUM_KEYS 256

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

// add a blank (defualt) key with the root for channel 0
key_entry lib[NUM_KEYS] = {{.key = {}, .start = 0, .level = ROOT_KEY_LEVEL}};
// initial values of lib pointers
key_entry* lib_chan[NUM_CHANNELS + 1] = {lib, lib+1, lib+1, lib+1, lib+1, lib+1};

#define LIB_END (lib + NUM_KEYS)

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
 * Returns true iff the key outer fully contains inner
 * i.e., inner could be derived from outer
 */
inline bool key_contains_key(key_entry outer, key_entry inner);

int derive_key(frame_number frame, channel_number chan, uint8_t key[32]) {
	// check if key is found in stack
	if (stksize[chan] > 0 && key_contains_frame(stk[chan][0], frame)) {
		goto key_found;
	}

	// try to find the correct key from the library
	for (key_entry* ptr = lib_chan[chan]; ptr < lib_chan[chan + 1]; ptr++) {
		if (key_contains_frame(*ptr, frame)) {
			stk[chan][0] = *ptr;
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
	frame = (frame >> entry.level - 1) << entry.level - 1;

	// call hash function
	hash_key(entry, frame, dst);
}

inline bool key_contains_frame(key_entry entry, frame_number frame) {
	return (entry.start >> entry.level) == (frame >> entry.level);
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

		if (key_contains_key(*slp, *ssp)) ssp++;
		else if (slp->start < ssp->start) slp++;
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
		
		if (elp[-1].start > esp[-1].start && !key_contains_key(esp[-1], elp[-1])) elp--;
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
		for (key_entry* ptr = lib_chan[NUM_CHANNELS] - 1; ptr >= elp; ptr--) {
			ptr[copy_dist] = ptr[0];
		}
	} else if (copy_dist < 0) {
		for (key_entry* ptr = elp; ptr < lib_chan[NUM_CHANNELS]; ptr++) {
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

inline bool key_contains_key(key_entry outer, key_entry inner) {
	if (outer.level < inner.level) return false;
	return (outer.start >> outer.level) == (inner.start >> outer.level);
	// return !((outer.start ^ inner.start) >> outer.level)
}