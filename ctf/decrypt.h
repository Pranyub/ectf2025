#pragma once

#include <stdint.h>

#include "common.h"

/**
 * Returns SUCCESS on success, FAILURE on failure
 * Returns key in key[]
 */
int derive_key(frame_number frame, channel_number chan, uint8_t key[32]);

/**
 * Adds a subscription to the library
 */
void add_subscription(struct subscription* sub);