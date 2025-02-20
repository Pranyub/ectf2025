#include <stdint.h>

// The level of the root key (leaf keys have level 1)
#define ROOT_KEY_LEVEL 64

#define MAX_KEYS_PER_SUBSCRIPTION 128

typedef struct {
  uint64_t frame_start;
  // level is the number of nodes below this one
  uint8_t level; // len = 1 << level
  uint8_t key[32];
} key_entry_t;

typedef struct {
  uint8_t active; // a boolean flag
  uint8_t nkeys;
  uint64_t frame_start;
  uint64_t frame_end;
  key_entry_t keys[MAX_KEYS_PER_SUBSCRIPTION];
} subscription_t;
