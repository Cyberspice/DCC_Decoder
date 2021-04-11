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
#include "dcc.h"

#define ICP1 PINB0

/* The time for a tick after prescaled */
#define TICK_US 1/2

/* Per NMRA standards */
#define BIT1_WIDTH_MIN_US  52
#define BIT1_WIDTH_MAX_US  62
#define BIT0_WIDTH_MIN_US  90
#define BIT0_WIDTH_MAX_US  10000

/* The ticks */
#define BIT1_WIDTH_MIN_TICKS   104
#define BIT1_WIDTH_MAX_TICKS   124
#define BIT0_WIDTH_MIN_TICKS   180
#define BIT0_WIDTH_MAX_TICKS 20000

typedef enum {
    DCC_BIT_TYPE_UNKNOWN,
    DCC_BIT_TYPE_0,
    DCC_BIT_TYPE_1
} DCC_BIT_TYPE;

typedef enum {
    DCC_BIT_STATE_UNKNOWN,
    DCC_BIT_STATE_FIRST_HALF,
    DCC_BIT_STATE_COMPLETE
} DCC_BIT_STATE;

typedef enum {
    DCC_PACKET_STATE_UNKNOWN,
    DCC_PACKET_STATE_PREAMBLE,
    DCC_PACKET_STATE_START_BIT,
    DCC_PACKET_STATE_DATA_BIT,
    DCC_PACKET_STATE_END_BIT,
    DCC_PACKET_STATE_DONE
} DCC_PACKET_STATE;

#define MIN_PREAMBLE_BIT_COUNT 12

static volatile bool bit_start_edge = true;
static volatile DCC_BIT_TYPE bit_type = DCC_BIT_TYPE_UNKNOWN;
static volatile DCC_BIT_STATE bit_state = DCC_BIT_STATE_UNKNOWN;
static volatile DCC_PACKET_STATE packet_state = DCC_PACKET_STATE_UNKNOWN;
static volatile uint8_t preamble_count = 0;
static volatile uint8_t packet_byte = 0;
static volatile uint8_t packet_byte_mask = 0x80;

uint8_t packet_data[MAX_PACKET_LEN];
volatile int packet_idx = 0;
volatile bool got_packet = false;

static inline void reset_states(void) {
    bit_start_edge = true;
    bit_type = DCC_BIT_TYPE_UNKNOWN;
    bit_state = DCC_BIT_STATE_UNKNOWN;
    packet_state = DCC_PACKET_STATE_UNKNOWN;
    packet_idx = 0;
    got_packet = false;
}

static inline bool process_bit(bool bit_is_1) {
    switch (packet_state) {
        case DCC_PACKET_STATE_UNKNOWN:
            if (!bit_is_1) {
                break;
            }

            preamble_count = 0;
            packet_state = DCC_PACKET_STATE_PREAMBLE;
            /* Drop through */
        case DCC_PACKET_STATE_PREAMBLE:
            /* Need at least MIN_PREAMBLE_BIT_COUNT 1 bits */
            if (bit_is_1) {
                preamble_count ++;
                return true;
            } else { /* 0 bit */
                 if (preamble_count < MIN_PREAMBLE_BIT_COUNT) {
                    /* We've not had enough 1 bits */
                    break;
                }

                /* The preamble ends with a zero bit as a byte start bit
                   as the start of the packet */
                packet_idx = 0;
                packet_state = DCC_PACKET_STATE_START_BIT;
            }
            /* Drop through */
        case DCC_PACKET_STATE_START_BIT:
        case DCC_PACKET_STATE_END_BIT:
            if (bit_is_1) {
                /* Packet length can vary but can't be more than
                   MAX_PACKET_LEN. So a bit 1 is an end of packet
                   and if its short, the packet validation will
                   detect that! */
                packet_state = DCC_PACKET_STATE_DONE;

                PORTB = PORTB & ~_BV(PB2);

                // Disable interrupts
                TIMSK1 = 0;
                got_packet = true;
                return true;
            } else {
                if (packet_state != DCC_PACKET_STATE_END_BIT) {
                    packet_byte = 0;
                    packet_byte_mask = 0x80;
                    packet_state = DCC_PACKET_STATE_DATA_BIT;
                    return true;
                } else {

                    /* It MUST be an end bit as we're at maximum
                       packet size! So too big! */
                    return false;
                }
            }
            break;
        case DCC_PACKET_STATE_DATA_BIT:
            if (bit_is_1) {
                packet_byte |= packet_byte_mask;
            }

            /* Big endian */
            packet_byte_mask >>= 1;

            /* If end of byte */
            if (packet_byte_mask == 0) {
                packet_data[packet_idx] = packet_byte;
                packet_idx ++;

                if (packet_idx >= MAX_PACKET_LEN) {
                    /* End of packet then we want the stop bit */
                    packet_state = DCC_PACKET_STATE_END_BIT;
                } else {
                    packet_state = DCC_PACKET_STATE_START_BIT;
                }
            }
            return true;
        case DCC_PACKET_STATE_DONE:
        default:
            packet_state = DCC_PACKET_STATE_UNKNOWN;
            break;
    }

    return false;
}

static inline bool process_edge(uint16_t width) {
    DCC_BIT_TYPE edge_type = DCC_BIT_TYPE_UNKNOWN;
    bool data_is_good = false;
    bool can_process_bit = false;

    if (width >= BIT1_WIDTH_MIN_TICKS && width <= BIT0_WIDTH_MAX_TICKS) {
        if (width < BIT1_WIDTH_MAX_TICKS) {
            edge_type = DCC_BIT_TYPE_1;
        }
        if (width >= BIT0_WIDTH_MIN_TICKS) {
            edge_type = DCC_BIT_TYPE_0;
        }
    }

    if (edge_type != DCC_BIT_TYPE_UNKNOWN) {
        data_is_good = true;

        switch (bit_state) {
            case DCC_BIT_STATE_UNKNOWN:
            case DCC_BIT_STATE_COMPLETE:
                /* This edge follows a good bit or unknown data, so its the
                first half of a bit. */
                bit_state = DCC_BIT_STATE_FIRST_HALF;
                break;

            case DCC_BIT_STATE_FIRST_HALF:
                if (packet_state == DCC_PACKET_STATE_PREAMBLE) {
                    /* Preamble is a special mode where we can synchronise. The
                       signal may be inverted in which case we have an odd
                       number of edges and what we think is the first edge is
                       actually the second. So if we get a bit 0 edge following
                       a bit 1 edge in this bit state we except the first half
                       of bit 1 as a full bit and the bit 0 edge as the start
                       of the following bit. */

                    /* bit_type is the type deduced from the previous edge */
                    if (edge_type == bit_type) {
                        bit_state = DCC_BIT_STATE_COMPLETE;
                        can_process_bit = true;
                    }
                } else {
                    /* The first and second half of the bit must be the same type */
                    if (edge_type == bit_type) {
                        bit_state = DCC_BIT_STATE_COMPLETE;
                        can_process_bit = true;
                    } else {
                        /* We've synchronised so this is an error */
                        data_is_good = false;
                    }
                }

                if (can_process_bit) {
                    data_is_good = process_bit(bit_type == DCC_BIT_TYPE_1);
                }
                /* Drop through */
            default:
                break;
        }

        bit_type = edge_type;
    }

    return data_is_good;
}

ISR (TIMER1_CAPT_vect) {
    uint16_t width = TCNT1;

    /* Clear the counter */
    TCNT1 = 0;

    /* Flip the edge bit */
    if (bit_start_edge) {
        TCCR1B = TCCR1B & ~_BV(ICES1);
    } else {
        TCCR1B = TCCR1B |= _BV(ICES1);
    }
    bit_start_edge = !bit_start_edge;

    /* If it was not good reset the state machine */
    if (!process_edge(width)) {
        reset_states();
    }
}

ISR (TIMER1_OVF_vect) {
    /* Clear the overflow bit */
    TIFR1 = TIFR1 | _BV(TOV1);

    /* Reset state machine */
    reset_states();
}

void init_icp1() {
    /* Normal mode */
    TCCR1A = 0;

    /* Prescaler of 1/8. Each pulse is 1/2 us */
    TCCR1B = _BV(CS11);

    /* Rising edge and noise cancelling */
    TCCR1B = TCCR1B | _BV(ICNC1) | _BV(ICES1);

    /* Normal mode */
    TCCR1C = 0;

    PORTB = PORTB | _BV(PB2);
    reset_states();

    /* Enable ICP1 interrupt and overflow interrupt */
    TIMSK1 = _BV(ICIE1) | _BV(TOIE1);

    /* Initialise the counter to 0 */
    TCNT1 = 0;

    /* Set up the port */
    PORTB = PORTB | _BV(ICP1);
    DDRB = DDRB & ~_BV(ICP1);
}
