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

#ifndef __DCCRX_H
#define __DCCRX_H

#include <stdint.h>
#include "dcc_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ICP1_DEBUG

/** \brief The packet data */
extern uint8_t packet_data[];

/** \brief The packet length. This is set at the end of the packet */
extern volatile uint8_t packet_len;

/**
 * \brief Initialise DCC reading.
 *
 * Set up the registers ready for read DCC. Does not start the
 * actual reading process.
 */
void dccrx_init(void);

/**
 * \brief Start reading DCC.
 *
 * Reads DCC from the input capture port. A packet is read when
 * packet_len is no zero in which case packet_data contains the
 * packet. Error checking will not have already taken place. Use
 * dccrx_isvalid() to check the packet validity.
 */
void dccrx_start(void);

/**
 * \brief Stop reading DCC.
 *
 * Stops the DCC packet reading process resetting the internal
 * state machine.
 */
void dccrx_stop(void);

/**
 * \brief tests the validity of a packet
 *
 * \param data the packet data
 * \param len the packet len
 *
 * \return true if valid
 */
bool dccrx_isvalid(uint8_t data[], uint8_t len);

#ifdef __cplusplus
}
#endif

#endif
