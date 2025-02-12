#ifndef CRYPTO_TESTS_H
#define CRYPTO_TESTS_H
#include "simple_flash.h"
#include "host_messaging.h"
#include "simple_uart.h"

#include "crypto.h"

static int decrypt_frame_test_normal(void);

int cyrpto_test_all(void) {

    return SUCCESS;
}

#endif
