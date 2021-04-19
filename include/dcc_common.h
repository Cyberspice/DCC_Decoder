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

#ifndef __DCC_COMMON_H
#define __DCC_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/* The time for a tick after prescaled */
#define TICK_US 1/2

/* Per NMRA standards */
#define BIT1_WIDTH_MIN_US  52
#define BIT1_WIDTH_US      58
#define BIT1_WIDTH_MAX_US  62
#define BIT0_WIDTH_MIN_US  90
#define BIT0_WIDTH_US      100
#define BIT0_WIDTH_MAX_US  10000

/* The ticks */
#define BIT1_WIDTH_MIN_TICKS   104
#define BIT1_WIDTH_TICKS       116
#define BIT1_WIDTH_MAX_TICKS   124
#define BIT0_WIDTH_MIN_TICKS   180
#define BIT0_WIDTH_TICKS       200
#define BIT0_WIDTH_MAX_TICKS 20000

/** The packet state during reading or writing */
typedef enum {
    DCC_PACKET_STATE_UNKNOWN,
    DCC_PACKET_STATE_PREAMBLE,
    DCC_PACKET_STATE_START_BIT,
    DCC_PACKET_STATE_DATA_BIT,
    DCC_PACKET_STATE_END_BIT,
    DCC_PACKET_STATE_DONE
} DCC_PACKET_STATE;

#define DCC_MAX_PACKET_LEN 6

/** A single packet with length and data */
typedef struct {
    uint8_t len;
    uint8_t packet[DCC_MAX_PACKET_LEN];
} DCC_PACKET_DATA;

#define DCC_BYTE_IDX_ADDRESS      0
#define DCC_BYTE_IDX_INSTRUCTION  1

#define DCC_ADDRESS_BROADCAST     0x00
#define DCC_ADDRESS_7BIT_MASK     0x7f
#define DCC_ADDRESS_ACCESSORY     0x80
#define DCC_ADDRESS_ACC_BROADCAST 0xbf
#define DCC_ADDRESS_MULTI_FUNC    0xc0
#define DCC_ADDRESS_EXTENDED_MASK 0xc0
#define DCC_ADDRESS_IDLE          0xff

/* For loco (multifunction) decoders */
#define DCC_INSTRUCTION_CONTROL   0x00
#define DCC_INSTRUCTION_ADVANCED  0x20
#define DCC_INSTRUCTION_FWD       0x40
#define DCC_INSTRUCTION_REV       0x60
#define DCC_INSTRUCTION_FUNC_1    0x80
#define DCC_INSTRUCTION_FUNC_2    0xa0
#define DCC_INSTRUCTION_RESERVED  0xc0
#define DCC_INSTRUCTION_CV        0xe0
#define DCC_INSTRUCTION_TYPE_MASK 0xe0
#define DCC_INSTRUCTION_DATA_MASK 0x1f

/* For accessory decoders */
#define DCC_ACC_BROADCAST_BASIC   0x80
#define DCC_ACC_BROADCAST_MASK    0xf0
#define DCC_ACC_BROADCAST_ADV     0x07

#ifdef __cplusplus
}
#endif

#endif