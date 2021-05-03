/*
Copyright 2021, Melanie Rhianna Lewis <cyberspice@cyberspice.org.uk>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __SERIALTX_H
#define __SERIALTX_H

#include <stdint.h>

// Uncomment to use interrupts for serial TX.
// #define SERIALTX_USE_INT

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initialise serial port 0
 *
 * This function initialises serial port 0 for 9600bps 8N1. It is for
 * debugging.
 */
void init_serial_0();

/**
 * \brief Send a single character to the serial port
 *
 * \param c the character
 */
void send_serial_0(uint8_t c);

/**
 * \brief Sends a string to the serial port
 *
 * \param str the string
 */
void send_serial_0_str(const char * str);

/**
 * \brief Utility function that converts a uint8_t value to a hex string
 *
 * This function populates the supplied buffer with the ASCII hex
 * representation of the supplied value. The buffer must be large enough
 * to hold two characters and a terminating NUL byte, i.e. 3 characters
 * long.
 *
 * \param value the value
 * \param str the string buffer
 */
void uint8_to_string(uint8_t value, char * str);

/**
 * \brief Utility function that converts a uint16_t value to a hex string
 *
 * This function populates the supplied buffer with the ASCII hex
 * representation of the supplied value. The buffer must be large enough
 * to hold two characters and a terminating NUL byte, i.e. 5 characters
 * long.
 *
 * \param value the value
 * \param str the string buffer
 */
void uint16_to_string(uint16_t value, char * str);

#ifdef __cplusplus
}
#endif

#endif
