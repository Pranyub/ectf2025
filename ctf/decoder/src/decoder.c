/**
 * @file    decoder.c
 * @author  Samuel Meyers
 * @brief   eCTF Decoder Example Design Implementation
 * @date    2025
 *
 * This source file is part of an example system for MITRE's 2025 Embedded System CTF (eCTF).
 * This code is being provided only for educational purposes for the 2025 MITRE eCTF competition,
 * and may not meet MITRE standards for quality. Use this code at your own risk!
 *
 * @copyright Copyright (c) 2025 The MITRE Corporation
 */

/*********************** INCLUDES *************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/sha256.h>

#include "mxc_device.h"
#include "status_led.h"
#include "board.h"
#include "mxc_delay.h"
#include "simple_flash.h"
#include "host_messaging.h"
#include "simple_uart.h"
#include "decoder.h"
#include "crypto.h"
#include "chacha20_tests.h"
#include "crypto_tests.h"

#define MAX_CHANNEL_COUNT 8

// Stores the current "chain" of keys that have been derived
static key_entry_t key_stack[MAX_CHANNEL_COUNT][ROOT_KEY_LEVEL];

// Keeps track of the current number of keys derived for each channel
static uint8_t key_stack_sz[MAX_CHANNEL_COUNT];

// Stores subscriptions for all channels
// Note: we add a zero root key for channel 0
// static subscription_t subscriptions[MAX_CHANNEL_COUNT] = { { .frame_start = 0, .frame_end = 0, .chan = 0, .nkeys = 1 } };

typedef struct {
    uint32_t first_boot; // if set to FLASH_FIRST_BOOT, device has booted before.
    subscription_t subscriptions[MAX_CHANNEL_COUNT];
} flash_entry_t;

flash_entry_t decoder_state;

/**
 * @brief Hashes an intermediate key to give the next key, which will eventually
 * yield a decryption key.
 * 
 * @param key the intermediate key
 * @param salt the salt to use to derive the next key
 * @param digest an output parameter that will store the resulting hash
 */
void key_hash(uint8_t key[32], uint64_t salt, uint8_t digest[32]) {
	Sha256 s;
	wc_InitSha256(&s);
    wc_Sha256Update(&s, key, 32);
	wc_Sha256Update(&s, (byte*)&salt, sizeof(salt));
    wc_Sha256Final(&s, digest);
}

/**
 * @brief Hashes the key to get the next key. Successive calls will eventually reach
 * the key for the given frame.
 * 
 * @param k the current key entry
 * @param frame the frame number whose decryption key we wish to derive
 * @param res an output parameter that will store the next key entry in the
 *        derivation
 */
void key_hash_towards_frame(key_entry_t* k, uint64_t frame, key_entry_t* res) {
    /*
     * The salt is the starting frame number covered by the key entry, which is
     * guaranteed to be unique for the left and right children. It is helpful
     * to think of the frame number's bitstring as deciding which route to take
     * (left vs right) at each level of the tree. Thus, the starting frame
     * number (salt) is the frame number with the lowest "level" bits set to
     * zero.
     * 
     * Note: the salt value was chosen somewhat arbitrarily; it does not matter
     * as long as it is the same for frames under the same subtree, and
     * distinct from the other salt at the same level.
     * 
     * Also note: level is assumed to be positive, since hashing a level 1 key
     * gives us the final key for the frame.
     */
    uint64_t child_level = k->level - 1;
    uint64_t child_frame_start = (frame >> child_level) << child_level;

    // Return a new key entry in the output parameter res
    res->frame_start = child_frame_start;
    res->level = child_level;
    key_hash(k->key, child_frame_start, res->key);
}

/**
 * @brief Returns true if the given key can eventually derive the given frame
 *
 * @param k a key
 * @param frame the frame number we wish to derive decryption keys for
 * @return 1 if k can eventually derive frame, 0 otherwise
 */
int key_covers_frame(key_entry_t* k, uint64_t frame) {
    /*
     * There are two cases: either the key's level is zero, or nonzero. If it's
     * zero, then it's a leaf key so the frame start must equal the given frame
     * number.
     * 
     * If the key's level is nonzero, then frame start must be a power of 2,
     * and everything from frame start to the start of the next level is
     * covered. This means we can ignore the low bits of the frame numbers
     * since they are part of the same subtree.
     * 
     * For example, let's say frame start is 16 and level is 4. In this case,
     * frames 16-31 (0b10000 - 0b11111) are covered by this key, so we can
     * ignore the last 4 bits during the comparison to find whether the
     * provided frame number lies in this range.
     * 
     * Now consider what if level is 3. In this case, frames 16-23
     * (0b10000 - 0b10111) are covered, so we can ignore the last 3 bits during
     * the comparison.
     */
    return (k->frame_start >> k->level) == (frame >> k->level);
}

/**
 * @brief
 * Returns the final decryption key for a particular frame
 *
 * @param chan the satellite channel number
 * @param frame the frame number we wish to decrypt
 * @param key output parameter that will hold the decryption key
 * @return 1 if key was successfully found, 0 otherwise
 */
int key_derive(uint8_t chan, uint64_t frame, uint8_t key[32]) {
    // the "stack" of derived keys so far: this is a macro to save typing
#define st (key_stack[chan])

    // the stack size
    uint8_t* sz = &key_stack_sz[chan];

    // boolean flag
    uint8_t key_found = 0;

    if (*sz <= 0) {
        // Empty stack: find correct intermediate key
        subscription_t* sub = &decoder_state.subscriptions[chan];
        for (int i = 0; i < sub->nkeys; i++) {
            if (key_covers_frame(&sub->keys[i], frame)) {
                // Setup the stack with the intermediate key
                st[0] = sub->keys[i];
                *sz = 1;

                key_found = 1;
                break;
            }
        }
    } else if (key_covers_frame(&st[0], frame)) {
        // It is possible, through repeated hashing, to derive a key for frame
        key_found = 1;
    }

    if (!key_found)
        return 0;

    // Pop the stack until the key on the top of the stack can be used to derive our frame
    while (!key_covers_frame(&st[*sz - 1], frame))
        (*sz)--;

    // Derive keys from the top of the stack until we get the level 0 key
    while (st[*sz - 1].level) {
        key_hash_towards_frame(&st[*sz - 1], frame, &st[*sz]);
        (*sz)++;
    }

    // Copy the top key in stack to output
    memcpy(key, st[*sz - 1].key, sizeof(((key_entry_t) {}).key));
    return 1;
}

/**********************************************************
 ******************* PRIMITIVE TYPES **********************
 **********************************************************/

#define timestamp_t uint64_t
#define channel_id_t uint32_t
#define decoder_id_t uint32_t
#define pkt_len_t uint16_t

/**********************************************************
 *********************** CONSTANTS ************************
 **********************************************************/

#define EMERGENCY_CHANNEL 0
#define FRAME_SIZE 64
#define DEFAULT_CHANNEL_TIMESTAMP 0xFFFFFFFFFFFFFFFF
// This is a canary value so we can confirm whether this decoder has booted before
#define FLASH_FIRST_BOOT 0xDEADBEEF

/**********************************************************
 ********************* STATE MACROS ***********************
 **********************************************************/

// Calculate the flash address where we will store channel info as the 8th to last page available
#define FLASH_STATUS_ADDR ((MXC_FLASH_MEM_BASE + MXC_FLASH_MEM_SIZE) - (8 * MXC_FLASH_PAGE_SIZE))


/**********************************************************
 *********** COMMUNICATION PACKET DEFINITIONS *************
 **********************************************************/

#pragma pack(push, 1) // Tells the compiler not to pad the struct members
// for more information on what struct padding does, see:
// https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Structure-Layout.html

// decrypted frame packet
typedef struct {
    uint8_t channel;
    uint64_t frame_number;
    uint8_t nonce[NONCE_SIZE];
    uint8_t data[FRAME_SIZE];
    uint8_t tag[TAG_LENGTH];
} frame_packet_t;

typedef struct {
    uint64_t frame_start;
    uint8_t level;
    uint8_t key[32];
} key_entry_packet_t;

typedef struct {
    uint8_t signature[ECDSA_SIGNATURE_SIZE];
    decoder_id_t decoder_id;
    uint8_t chan;
    uint8_t nkeys;
    uint64_t frame_start;
    uint64_t frame_end;
    key_entry_packet_t keys[MAX_KEYS_PER_SUBSCRIPTION];
} subscription_update_packet_t;

// These structs are just for responding to the host according with the correct format
typedef struct {
    uint32_t channel;
    uint64_t start;
    uint64_t end;
} channel_info_t;

typedef struct {
    uint32_t n_channels;
    channel_info_t channel_info[MAX_CHANNEL_COUNT];
} list_response_t;

#pragma pack(pop) // Tells the compiler to resume padding struct members

/**********************************************************
 ******************* UTILITY FUNCTIONS ********************
 **********************************************************/

subscription_t* subscription_get(uint32_t chan) {
    for (int i = 0; i < MAX_CHANNEL_COUNT; i++) {
        if (decoder_state.subscriptions[i].chan == chan && decoder_state.subscriptions[i].active) {
            return &decoder_state.subscriptions[i];
        }
    }
    return NULL;
}

/** @brief Checks whether the decoder is subscribed to a given channel
 *
 *  @param channel The channel number to be checked.
 *  @return 1 if the the decoder is subscribed to the channel.  0 if not.
*/
int is_subscribed(uint32_t chan) {
    // Check if this is an emergency broadcast message
    if (chan == EMERGENCY_CHANNEL) {
        return 1;
    }

    // Check if the decoder has has a subscription
    return subscription_get(chan) != NULL;
}

/**********************************************************
 ********************* CORE FUNCTIONS *********************
 **********************************************************/

/** @brief Lists out the actively subscribed channels over UART.
 *
 *  @return 0 if successful.
*/
int list_channels() {
    list_response_t resp;
    pkt_len_t len;

    resp.n_channels = 0;

    for (uint32_t i = 0; i < MAX_CHANNEL_COUNT; i++) {
        if (decoder_state.subscriptions[i].active) {
            resp.channel_info[resp.n_channels].channel =  decoder_state.subscriptions[i].chan;
            resp.channel_info[resp.n_channels].start = decoder_state.subscriptions[i].frame_start;
            resp.channel_info[resp.n_channels].end = decoder_state.subscriptions[i].frame_end;
            resp.n_channels++;
        }
    }

    len = sizeof(resp.n_channels) + (sizeof(channel_info_t) * resp.n_channels);

    // Success message
    write_packet(LIST_MSG, &resp, len);
    return 0;
}

void flash_reset(void) {
    uint32_t addr = FLASH_STATUS_ADDR;
    for (int i = 0; i < 8; i++) {
        flash_simple_erase_page(addr);
        addr += MXC_FLASH_PAGE_SIZE;
    }
}

/** @brief Updates the channel subscription for a subset of channels.
 *
 *  @param pkt_len The length of the incoming packet
 *  @param update A pointer to an array of channel_update structs,
 *      which contains the channel number, start, and end timestamps
 *      for each channel being updated.
 *
 *  @note Take care to note that this system is little endian.
 *
 *  @return 0 upon success.  -1 if error.
 */
int update_subscription(uint16_t pkt_len, subscription_update_packet_t *update) {
    if (update->chan == EMERGENCY_CHANNEL) {
        STATUS_LED_RED();
        print_error("Failed to update subscription - cannot subscribe to emergency channel\n");
        return -1;
    }

    // verify the subscription signature
    
    // create flat array of subscription data to be verified
    uint8_t flattened[sizeof()];
    for (int i = 0; i < update->nkeys; i++) {

    }
    if (ecdsa_verify(update->keys, )) {

    }

    // Find the first empty slot in the subscription array
    int i;
    for (i = 0; i < MAX_CHANNEL_COUNT; i++) {
        // Current subscription
        subscription_t* sub = &(decoder_state.subscriptions[i]);

        if (sub->chan == update->chan || !sub->active) {
            sub->active = true;
            sub->chan = update->chan;
            sub->frame_start = update->frame_start;
            sub->frame_end = update->frame_end;
            sub->nkeys = update->nkeys;

            // Copy the keys
            for (int j = 0; j < update->nkeys; j++) {    
                sub->keys[j].frame_start = update->keys[j].frame_start;
                sub->keys[j].level = update->keys[j].level;

                memcpy(&sub->keys[j].key, &update->keys[j].key, 32);
            }

            break;
        }
    }

    // If we do not have any room for more subscriptions
    if (i == MAX_CHANNEL_COUNT) {
        STATUS_LED_RED();
        print_error("Failed to update subscription - max subscriptions installed\n");
        return -1;
    }

    flash_reset();
    flash_simple_write(FLASH_STATUS_ADDR, &decoder_state, sizeof(flash_entry_t));
    print_debug("Wrote subscription to flash!");

    // Success message with an empty body
    write_packet(SUBSCRIBE_MSG, NULL, 0);
    return 0;
}

/** @brief Processes a packet containing frame data.
 *
 *  @param pkt_len A pointer to the incoming packet.
 *  @param new_frame A pointer to the incoming packet.
 *
 *  @return 0 if successful. -1 if data is from unsubscribed channel.
 */
int decode(pkt_len_t pkt_len, frame_packet_t *new_frame) {
    char output_buf[128] = {0};

    uint8_t chan = new_frame->channel;
    uint64_t frame = new_frame->frame_number;

    print_debug("Checking subscription\n");
    subscription_t *sub = subscription_get(chan);

    if (is_subscribed(chan)
            && frame >= sub->frame_start
            && frame <= sub->frame_end) {
        print_debug("Subscription Valid\n");        
        
        uint8_t key[32];
        memset(key, 0, sizeof(key));
        if (!key_derive(chan, frame, key)) {
            sprintf(output_buf, "Decoding frame #%u failed: could not derive key\n", frame);
            print_error(output_buf);
            return -1;
        }

        uint8_t decoded_frame[FRAME_SIZE];
        if (decrypt_frame(key, chan, frame, new_frame->nonce, new_frame->data, new_frame->tag, decoded_frame)) {
            // error
            sprintf(output_buf, "Decoding frame #%u failed: could not decrypt frame\n", frame);
            print_error(output_buf);
            return -1;
        }

        write_packet(DECODE_MSG, decoded_frame, FRAME_SIZE);
        return 0;
    } else {
        STATUS_LED_RED();
        sprintf(output_buf, "Receiving unsubscribed channel data. %u\n", chan);
        print_error(output_buf);
        return -1;
    }
}

/** @brief Initializes peripherals for system boot.
 */
void init() {
    int ret;

    // Initialize the flash peripheral to enable access to persistent memory
    flash_simple_init();

    // Read starting flash values into our flash status struct
    flash_simple_read(FLASH_STATUS_ADDR, &decoder_state, sizeof(flash_entry_t));
    if (decoder_state.first_boot != FLASH_FIRST_BOOT) {
        /* If this is the first boot of this decoder, mark all channels as unsubscribed.
         * This data will be persistent across reboots of the decoder. Whenever the decoder
         * processes a subscription update, this data will be updated.
         */
        print_debug("First boot.  Setting flash...\n");

        decoder_state.first_boot = FLASH_FIRST_BOOT;

        subscription_t subs[MAX_CHANNEL_COUNT];
        memset(subs, 0, sizeof(subscription_t) * MAX_CHANNEL_COUNT);

        for (int i = 0; i < MAX_CHANNEL_COUNT; i++) {
            // TODO: check this
            subs[i].chan = 0;
            subs[i].frame_start = DEFAULT_CHANNEL_TIMESTAMP;
            subs[i].frame_end = DEFAULT_CHANNEL_TIMESTAMP;
            subs[i].nkeys = 0;
            subs[i].active = false;
        }

        // Write the starting channel subscriptions into flash.
        memcpy(&decoder_state.subscriptions, subs, MAX_CHANNEL_COUNT*sizeof(subscription_t));
        flash_simple_erase_page(FLASH_STATUS_ADDR);
        flash_simple_write(FLASH_STATUS_ADDR, &decoder_state, sizeof(flash_entry_t));
    }

    // Initialize the uart peripheral to enable serial I/O
    ret = uart_init();
    if (ret < 0) {
        STATUS_LED_ERROR();
        // if uart fails to initialize, do not continue to execute
        while (1);
    }
}

int main(void) {
    char output_buf[128] = {0};
    uint8_t uart_buf[100];
    msg_type_t cmd;
    int result;
    uint16_t pkt_len;

    // initialize the device
    init();

    print_debug("Decoder Booted!\n");
    
    // process commands forever
    while (1) {
        print_debug("Ready\n");

        STATUS_LED_GREEN();

        result = read_packet(&cmd, uart_buf, &pkt_len);

        if (result < 0) {
            STATUS_LED_ERROR();
            print_error("Failed to receive cmd from host\n");
            continue;
        }

        // Handle the requested command
        switch (cmd) {

        // Handle list command
        case LIST_MSG:
            STATUS_LED_CYAN();

            // tests
            chacha20_test_all();
            crypto_test_all();
            list_channels();
            break;

        // Handle decode command
        case DECODE_MSG:
            STATUS_LED_PURPLE();
            decode(pkt_len, (frame_packet_t *)uart_buf);
            break;

        // Handle subscribe command
        case SUBSCRIBE_MSG:
            STATUS_LED_YELLOW();
            update_subscription(pkt_len, (subscription_update_packet_t *)uart_buf);
            break;

        // Handle bad command
        default:
            STATUS_LED_ERROR();
            sprintf(output_buf, "Invalid Command: %c\n", cmd);
            print_error(output_buf);
            break;
        }
    }
}
