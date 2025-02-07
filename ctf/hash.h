#pragma once

#include "common.h"

/**
 * Hashes the source key with the start value of the dstkey to produce the value of the dstkey
 * outputs the dstkey into dstkey
 */
void hash_key(key_entry srckey, uint64_t dstkey_start, key_entry* dstkey);
