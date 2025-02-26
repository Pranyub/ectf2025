#ifndef CONSTANTS_H
#define CONSTANTS_H

// DECODER.H
#define timestamp_t uint64_t
#define channel_id_t uint32_t
#define decoder_id_t uint32_t
#define pkt_len_t uint16_t

#define MAX_CHANNEL_COUNT 8
#define NUM_SUBSCRIPTIONS

#define EMERGENCY_CHANNEL 0
#define FRAME_SIZE 64
#define DEFAULT_CHANNEL_TIMESTAMP 0xFFFFFFFFFFFFFFFF
// This is a canary value so we can confirm whether this decoder has booted before
#define FLASH_FIRST_BOOT 0xDEADBEEF

// Calculate the flash address where we will store channel info as the 8th to last page available
#define FLASH_NUM_PAGES 8
#ifdef LINUX_BUILD
#define MXC_FLASH_MEM_BASE 0
#define MXC_FLASH_MEM_SIZE 128000  // doesn't have to be the actual size of memory
#define MXC_FLASH_PAGE_SIZE 0x2000
#endif
#define FLASH_STATUS_ADDR ((MXC_FLASH_MEM_BASE + MXC_FLASH_MEM_SIZE) - (8 * MXC_FLASH_PAGE_SIZE))

// The level of the root key (leaf keys have level 1)
#define ROOT_KEY_LEVEL 64
#define MAX_KEYS_PER_SUBSCRIPTION 128

// CRYPTO.H
#define TAG_LENGTH 16
#define CHACHA_KEY_SIZE 32
#define NONCE_SIZE 12
#define CRYPTO_FRAME_SIZE 64

#define ECDSA_SIGNATURE_SIZE 64
#define CRYPTO_ECC_KEY_SIZE 65
#define ECC_CURVE ECC_SECP256K1

#define SUCCESS 0
#define ERROR 1
#define TAG_ERROR 2
#define ECDSA_VERIFY_ERROR 3

#endif
