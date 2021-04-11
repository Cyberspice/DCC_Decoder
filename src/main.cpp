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

#include <avr/sleep.h>
#include <util/atomic.h>

#ifndef INC_ARDUINO_H
#include <util/delay.h>
#endif

#include "serialtx.h"
#include "heartbeat.h"

#include "dcc.h"

static char nybbleToHex(uint16_t nybble) {
    if (nybble < 10) {
        return '0' + nybble;
    } else {
        return 'a' + (nybble - 10);
    }
}

static void uint16ToString(uint16_t value, char * str) {
    *str++ = nybbleToHex((value & 0xf000) >> 12);
    *str++ = nybbleToHex((value & 0x0f00) >> 8);
    *str++ = nybbleToHex((value & 0x00f0) >> 4);
    *str++ = nybbleToHex((value & 0x000f) >> 0);
    *str++ = 0;
}

static void init_diag_led(void) {
    DDRB = DDRB | _BV(DDB2);
    PORTB = PORTB | _BV(PB2);
}

void setup() {
    init_builtin_led();
    init_timer_0();
    init_serial_0();
    init_icp1();
    init_diag_led();

    send_serial_0_str("DCC code 0.05\n");
}

char str[6];

#define _NOP() do { __asm__ __volatile__ ("nop"); } while (0)

void loop() {
    if (!got_packet) return;
    
    for (int i = 0; i < packet_idx; i++) {
        uint16ToString(packet_data[i], str);
        send_serial_0_str(str);
        send_serial_0(' ');
    }
    send_serial_0('\n');
    init_icp1();
}

int main(void) {
    setup();
    sei();
    while(true) {
        loop();
    }
}
