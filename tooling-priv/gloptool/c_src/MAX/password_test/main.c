/******************************************************************************
 *
 * Copyright (C) 2022-2023 Maxim Integrated Products, Inc. (now owned by 
 * Analog Devices, Inc.),
 * Copyright (C) 2023-2024 Analog Devices, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

/**
 * @file    main.c
 * @brief   Password test.
 * @details This example uses the UART to prompt and check a password.
 */

/***** Includes *****/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "mxc_device.h"
#include "board.h"
#include "mxc_delay.h"

/***** Definitions *****/

/***** Globals *****/

/***** Functions *****/

// *****************************************************************************
int main(void)
{
    puts("Starting program\n");
    char password[50] = "\0";
    int length = 0;
    while (strncmp(password, "supersec", 50) != 0) {
        puts("Enter the password: ");
        while (length < 49) {
            char character = getchar();
            if (character == EOF || character == '\r' || character == '\n' || character == '\0') break;
            password[length] = character;
            length++;
        }
        password[length] = '\0';
        puts("\n");
    }
    puts("Password correct.\r\n");
    while (1) {}
}
