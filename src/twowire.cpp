
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
#include <util/twi.h>
#include "twowire.h"

/** 100 KHZ clock frequency */
#define TW_FREQUENCY 100000

/** With a Prescaler of 1 -> (((F_CPU / TW_FREQUENCY) - 16) / 2) */
#define TW_TWBR_VAL 72

/** Wait for the transmit */
#define TW_WAITTX() while ((TWCR & _BV(TWINT)) == 0)

/** Number of times to try and start transmission */
#define TW_MAX_RETRIES 3

void twowire_init() {
    // Turn off pullups
    PORTC = PORTC &~ (_BV(PC4) | _BV(PC5));

    // CLock
    TWBR = TW_TWBR_VAL;
}

static inline uint8_t twowire_sendstart() {
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    TW_WAITTX();
    return TW_STATUS;
}

static inline void twowire_sendstop() {
    TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

static inline uint8_t twowire_sendbyte(uint8_t b) {
    TWDR = b;
    TWCR = _BV(TWINT) | _BV(TWEN);
    TW_WAITTX();
    return TW_STATUS;
}

static inline uint8_t twowire_sendaddr(uint8_t addr, uint8_t mode) {
    return twowire_sendbyte(addr | mode);
}

uint8_t twowire_writebytes(uint8_t addr, uint8_t len, uint8_t bytes[], bool send_stop) {
    uint8_t status = TWOWIRE_ERROR;

    // Repeat until we have arbitration
    do {
        status = twowire_sendstart();
        switch (status) {
            case TW_REP_START:
            case TW_START:
                break;  
            case TW_MT_ARB_LOST:
                continue; // while
            default:
                return TWOWIRE_ERROR; // Don't send stop bit
        }

        status = twowire_sendaddr(addr, TW_WRITE);
        switch (status) {
            case TW_MT_SLA_ACK:
                break;
            case TW_MT_SLA_NACK:
                break;
            case TW_MT_ARB_LOST:
                continue; // the while
            default:
                status = TWOWIRE_ERROR;
                break;
        }
    } while (status == TW_MT_ARB_LOST);

    // Now write data
    if (status == TW_MT_SLA_ACK) {
        for (uint8_t i = 0; i < len; i++) {
            status = twowire_sendbyte(bytes[i]);
            switch (status) {
                case TW_MT_DATA_ACK:
                    break;
                default:
                    status = TWOWIRE_ERROR;
                    break;
            }

            if (status != TW_MT_DATA_ACK) {
                break;
            }
        }
    }

    if (send_stop) {
        twowire_sendstop();
    }

    return status;
}

uint8_t twowire_readbytes(uint8_t addr, uint8_t len, uint8_t bytes[]) {
    uint8_t status = TWOWIRE_ERROR;

    do {
        status = twowire_sendstart();
        switch (status) {
            case TW_REP_START:
            case TW_START:
                break;  
            case TW_MT_ARB_LOST:
                continue; // while
            default:
                return TWOWIRE_ERROR; // Don't send stop bit
        }

        status = twowire_sendaddr(addr, TW_READ);
        switch (status) {
            case TW_MR_SLA_ACK:
            case TW_MR_SLA_NACK:
                break;
            case TW_MR_ARB_LOST:
                continue; // the while
            default:
                status = TWOWIRE_ERROR;
                break;
        }

    } while (status == TW_MT_ARB_LOST); // TW_MT_ARB_LOST == TW_MR_ARB_LOST

    if (status == TW_MR_SLA_ACK) {
        for (uint8_t i = 0; i < len; i++) {
            TWCR = _BV(TWINT) | _BV(TWEN);
            TW_WAITTX();

            status = TW_STATUS;
            switch (status) {
                case TW_MR_DATA_ACK:
                case TW_MR_DATA_NACK:
                    bytes[i] = TWDR;
                    break;
                default:
                    status = TWOWIRE_ERROR;
                    break;
            }

            if (status != TW_MR_DATA_ACK) {
                break;
            }
        }   
    }

    twowire_sendstop();
    return status;
}
