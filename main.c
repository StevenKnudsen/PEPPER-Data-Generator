// Copyright (c) 2023 Michael Stoops. All rights reserved.
// Portions copyright (c) 2021 Raspberry Pi (Trading) Ltd.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//    products derived from this software without specific prior written
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// SPDX-License-Identifier: BSD-3-Clause
//
// Example of an SPI bus master using the PL022 SPI interface

//
// Modified by Steven Knudsen to emulate the PEPPER FPGA SPI data stream.

#include "hardware/spi.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdio.h>

#define MAN_CS_PIN 20 // GP20
#define BUF_LEN 4     // 24 bits sampled plus 8 zero-bits 0xNNNNNN00

void printbuf(uint8_t buf[], size_t len) {
  int i;
  for (i = 0; i < len; ++i) {
    if (i % 16 == 15)
      printf("%02x\n", buf[i]);
    else
      printf("%02x ", buf[i]);
  }

  // append trailing newline if there isn't one
  if (i % 16) {
    putchar('\n');
  }
}

int main() {
  // Enable UART so we can print
  stdio_init_all();
#if !defined(spi_default) || !defined(PICO_DEFAULT_SPI_SCK_PIN) ||             \
    !defined(PICO_DEFAULT_SPI_TX_PIN) || !defined(PICO_DEFAULT_SPI_RX_PIN) ||  \
    !defined(PICO_DEFAULT_SPI_CSN_PIN)
#warning spi/spi_master example requires a board with SPI pins
  puts("Default SPI pins were not defined");
#else

  printf("SPI master example\n");

  // Enable SPI 0 at 1 MHz and connect to GPIOs
  spi_init(spi_default, 6000 * 1000);
  gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
  gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
  gpio_set_function(PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI);
  // Make the SPI pins available to picotool
  bi_decl(bi_4pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN,
                             PICO_DEFAULT_SPI_SCK_PIN, PICO_DEFAULT_SPI_CSN_PIN,
                             GPIO_FUNC_SPI));

  // We are not going to connect the PICO_DEFAULT_SPI_CSN_PIN.
  // Instead, we use our own output pin, GP20, as chip select
  // and toggle it so that 4 bytes at a time are sent.

  gpio_init(MAN_CS_PIN);
  gpio_set_dir(MAN_CS_PIN, GPIO_OUT);
  gpio_put(MAN_CS_PIN, 1);

  uint8_t out_buf[BUF_LEN];

  out_buf[3] = 0; // last byte always 0
  while (1) {
    for (size_t i = 0; i < 256 / (BUF_LEN - 1); i++) {
      printf("baud rate %d\n", spi_get_baudrate(spi_default));
      out_buf[0] = i * (BUF_LEN - 1) + 0;
      out_buf[1] = i * (BUF_LEN - 1) + 1;
      out_buf[2] = i * (BUF_LEN - 1) + 2;
      out_buf[3] = 0;
      // Write the output buffer to MOSI, and at the same time read from MISO.
      gpio_put(MAN_CS_PIN, 0);
      spi_write_blocking(spi_default, out_buf, BUF_LEN);
      gpio_put(MAN_CS_PIN, 1);

//    sleep_ms(250);
    }
  }
#endif
}
