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

#include <avr/io.h>
#include "serialtx.h"

void init_serial_0() {
    UBRR0H = 0;
    UBRR0L = 16;

    // Double speed
    UCSR0A = UCSR0A | _BV(U2X0);

    // Bits, parity and stop
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);

    // Tx/Rx settings (TX only)
    UCSR0B = _BV(TXEN0);
}

void send_serial_0(uint8_t c) {
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

void send_serial_0_str(const char * str) {
    for (const char *pc = str; *pc; pc++) {
        send_serial_0((uint8_t)*pc);
    }
}

static char nybble_to_hex(uint8_t nybble) {
    if (nybble < 10) {
        return '0' + nybble;
    } else {
        return 'a' + (nybble - 10);
    }
}

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
void uint8_to_string(uint8_t value, char * str) {
    *str++ = nybble_to_hex((value & 0x00f0) >> 4);
    *str++ = nybble_to_hex((value & 0x000f) >> 0);
    *str++ = 0;
}
