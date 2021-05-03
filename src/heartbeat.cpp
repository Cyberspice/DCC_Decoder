
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
#include "heartbeat.h"

#define MS_DELAY 1000

volatile uint16_t count_ms = 0;

#ifdef HEARTBEAT_LED
static volatile uint16_t count = 0;
static volatile bool led_on = false;
#endif

ISR (TIMER0_COMPA_vect) {
    TCNT0 = 0;
    TIFR0 = TIFR0 | _BV(OCR0A);

    count_ms++;

#ifdef HEARTBEAT_LED
    count++;

    // Half a second
    if (count >= 500) {
        count = 0;
        if (led_on) {
        led_on = false;
        PORTB = PORTB & ~_BV(PORTB5);
        } else {
        led_on = true;
        PORTB = PORTB | _BV(PORTB5);
        }
    }
#endif
}

void init_heartbeat() {
    // Divide by 64
    TCCR0B = _BV(CS01) | _BV(CS00);
    // Set timer to 0
    TCNT0 = 0;
    // Interrupt every 1ms
    OCR0A = 250;
    // Enable OC0A interrupt
    TIMSK0 |= _BV(OCIE0A);
}

#ifdef HEARTBEAT_LED
void init_builtin_led(void) {
    DDRB = DDRB | _BV(DDB5);
    PORTB = PORTB | _BV(PORTB5);
}
#endif
