#pragma once

#include <stdint.h>

#include "common.h"

/**
 * Returns SUCCESS on success, FAILURE on failure
 */
int derive_key(uint64_t packet_id, uint32_t channel_no, uint8_t key[32]);

/**
 * Adds a subscription to the library
 */
void add_subscription(struct subscription* sub);