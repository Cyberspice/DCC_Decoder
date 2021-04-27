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
#include "mcp23008.h"
#include "heartbeat.h"

#include "dccrx.h"

char str[6];

static void init_diag_led(void) {
    DDRB = DDRB | _BV(DDB2);
    PORTB = PORTB | _BV(PB2);
}

void setup() {
    init_builtin_led();
    init_timer_0();
    init_serial_0();
    dccrx_init();
    init_diag_led();
    twowire_init();
    mcp23008_init(0x40);

    /* Configure sleep */
    set_sleep_mode(SLEEP_MODE_IDLE);

    /* A hello */
    send_serial_0_str("DCC code 0.06\n");
    dccrx_start();
}

uint8_t prev_packet[DCC_MAX_PACKET_LEN];
uint8_t prev_packet_len = 0;

void print_packet() {
    for (uint8_t i = 0; i < prev_packet_len; i++) {
        uint8_to_string(prev_packet[i], str);
        send_serial_0_str(str);
        send_serial_0(' ');
    }
    send_serial_0('\n');
}

void loop() {
    if (packet_len > 0) {
        bool different = false;

        /* Different packet if different length or different bytes */
        if (packet_len != prev_packet_len) {
            different = true;
        } else {
            for (uint8_t i = 0; i < packet_len; i++) {
                if (packet_data[i] != prev_packet[i]) {
                    different = true;
                    break;
                }
            }
        }

        /* Copy if different */
        if (different) {
            prev_packet_len = packet_len;
            for (uint8_t i = 0; i < packet_len; i++) {
                prev_packet[i] = packet_data[i];
            }
        }

        /* Start collecting next one */
        dccrx_start();

        /* Print current one */
        if (different) {
            print_packet();
        }
    }
    sleep_mode();
}

int main(void) {
    setup();
    sei();
    while(true) {
        loop();
    }
}
