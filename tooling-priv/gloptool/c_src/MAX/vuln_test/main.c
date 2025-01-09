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

void __attribute__ ((noinline)) vuln(void) {
  char foo[4];
  char c;
  int i = 0;
  volatile int n = 4;
  for (i = 0; i < n; i++) {
    c = getchar();
    foo[i] = c;
    //if (c == EOF || c == '\r' || c == '\n' || c == '\0') break;
  }
  puts("finished");
  puts(foo);
}

// *****************************************************************************
int main(void)
{
    puts("begin");
    vuln();
    while (1) {}
}
