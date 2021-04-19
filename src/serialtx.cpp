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
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "serialtx.h"

#define CBUF_SIZE 16
uint8_t cbuf[CBUF_SIZE];
volatile uint8_t cbufw = 0;
volatile uint8_t cbufr = 0;
volatile bool ints = false;

ISR(USART_UDRE_vect) {
    /* Send next character */
    uint8_t c = cbuf[cbufr];
    cbufr = (cbufr + 1) % CBUF_SIZE;

    UDR0 = c;

    /* Clear TXC0 - Done by writing a 1 to the bit! */
    UCSR0A = UCSR0A | _BV(TXC0);

    /* If empty disable interrupts */
    if (cbufr == cbufw) {
        UCSR0B = UCSR0B & ~_BV(UDRIE0);
        ints = false;
    }
}

void init_serial_0() {
    // (F_CPU / 4 / 115200 - 1) / 2 = 16
    // (F_CPU / 8 / 9600 - 1) / 2 = 103
    UBRR0H = 0;
    UBRR0L = 103;

    // Double speed
    //UCSR0A = UCSR0A | _BV(U2X0);

    // Bits, parity and stop
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);

    // Tx/Rx settings (TX only)
    UCSR0B = _BV(TXEN0);
}

void send_serial_0(uint8_t c) {
    // Buffer is empty (int not enabled), just send byte
    if ((cbufw == cbufr) && bit_is_set(UCSR0A, UDRE0)) {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            UDR0 = c;
            /* Clear TXC0 - Done by writing a 1 to the bit! */
            UCSR0A = UCSR0A | _BV(TXC0);
        }
        return;
    }

    // Else stick it in the buffer
    uint8_t cbufn = (cbufw + 1) % CBUF_SIZE;

    /* Buffer is full, wait for it to empty  */
    while (cbufn == cbufr) {
        /* Do nothing, ISR is doing it! */
    }

    cbuf[cbufw] = c;

    if (ints) {
        cbufw = cbufn;
    } else {
        // Enable interrupts
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            cbufw = cbufn;
            UCSR0B = UCSR0B | _BV(UDRIE0);
            ints = true;
        }
    }
}

void send_serial_0_str(const char * str) {
    for (const char *pc = str; *pc; pc++) {
        send_serial_0((uint8_t)*pc);
    }
    _delay_ms(0);
}

static char nybble_to_hex(uint8_t nybble) {
    if (nybble < 10) {
        return '0' + nybble;
    } else {
        return 'a' + (nybble - 10);
    }
}

void uint8_to_string(uint8_t value, char * str) {
    *str++ = nybble_to_hex((value & 0x00f0) >> 4);
    *str++ = nybble_to_hex((value & 0x000f) >> 0);
    *str++ = 0;
}
