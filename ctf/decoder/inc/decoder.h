#include <stdint.h>

#define NUM_CHANNELS 5

// The level of the root key (leaf keys have level 1)
#define ROOT_KEY_LEVEL 64

#define MAX_KEYS_PER_SUBSCRIPTION 30

typedef struct {
  uint8_t key[32];
  uint64_t frame_start;

  // level is the number of nodes below this one
  uint8_t level; // len = 1 << level
} key_entry_t;

typedef struct {
  uint64_t frame_start;
  uint64_t frame_end;
  key_entry_t keys[MAX_KEYS_PER_SUBSCRIPTION];
  uint8_t chan;
  uint8_t nkeys;
} subscription_t;
