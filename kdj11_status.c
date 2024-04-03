/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (C) Andrei Warkentin <andreiw@mm.st>
 *
 * Report system progress/load on KDJ11-D/S cab kit
 * 8-segment display, which displays digits [0:F].
 *
 * Great way to tell if the system is alive or wedged
 * up, 'cause you never know. The value switches every
 * second between 0 and the scaled load average value,
 * between 1-14(hex E), with 15(hex F) reported for
 * values larger than 14. Basically, a load average of
 * 1.0 is reported as a 7.
 *
 * TODO: support KDJ11-B, which uses two 8-segment
 * displays. Support other J11-based boards.
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <sys/errno.h>
#include <sys/types.h>

#define BASE_ADDR 0177775
#define SEG_REG 6
#define IO_SIZE 1

main (argc, argv)
     int argc;
     char *argv[];
{
  unsigned char *io_ptr;

  if (phys (SEG_REG, IO_SIZE, BASE_ADDR) == -1) {
    err (1, "phys failed");
  }

  if (daemon (0, 0) == -1) {
    err (1, "daemon failed");
  }

  /*
   * Set up the page address pointer.  This is written to allow simple
   * changing of the #defines at the start of the program to ripple through
   * to the rest of the code.  Do not allow 0 because that would collide
   * with the first 8kb of code or data.  Do not allow 7 because that would
   * collide with our stack.
   */
  assert(SEG_REG > 0 && SEG_REG < 7);
  io_ptr = (unsigned char *)((u_int)020000 * SEG_REG);

  while (1) {
    double avenrun;
    short value;

    *(unsigned short *) (io_ptr + 020) = 0;
    sleep(1);

    getloadavg(&avenrun, 1);
    value = (short)(avenrun * 6) + 1;
    if (value > 0xe) {
      value = 0xf;
    }
    *(unsigned short *) (io_ptr + 020) = value;
    sleep(1);

  }
  exit(0);
}
