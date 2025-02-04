#pragma once

#include <stdint.h>

#include "common.h"

/**
 * Hashes the next block of keys starting at the given frame number
 * Stores results in output parameter dst
 */
void hash_block(key_block* dst, frame_number block);

/**
 * adds the subscription to the decoder
 */
void add_subscription(subscription* sub);