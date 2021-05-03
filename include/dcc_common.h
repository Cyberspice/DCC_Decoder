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

#define DCC_BYTE_IDX_ADDRESS             0
#define DCC_BYTE_IDX_INSTRUCTION         1

#define DCC_ADDRESS_BROADCAST         0x00
#define DCC_ADDRESS_7BIT_MASK         0x7f
#define DCC_ADDRESS_ACCESSORY         0x80
#define DCC_ADDRESS_ACCESSORY_MASK    0xc0
#define DCC_ADDRESS_MULTI_FUNC        0xc0
#define DCC_ADDRESS_MULTI_FUNC_MASK   0xc0
#define DCC_ADDRESS_IDLE              0xff

#define DCC_BROADCAST_LEN                3
#define DCC_IDLE_LEN                     3

/* For loco (multifunction) decoders */
#define DCC_INSTRUCTION_CONTROL       0x00
#define DCC_INSTRUCTION_ADVANCED      0x20
#define DCC_INSTRUCTION_FWD           0x40
#define DCC_INSTRUCTION_REV           0x60
#define DCC_INSTRUCTION_FUNC_1        0x80
#define DCC_INSTRUCTION_FUNC_2        0xa0
#define DCC_INSTRUCTION_RESERVED      0xc0
#define DCC_INSTRUCTION_CV            0xe0
#define DCC_INSTRUCTION_TYPE_MASK     0xe0
#define DCC_INSTRUCTION_DATA_MASK     0x1f

/* For accessory decoders */
#define DCC_ACC_ADDRESS_MASK_LOW      0x3f
#define DCC_ACC_BASIC_PACKET_BIT      0x80
#define DCC_ACC_ADDRESS_MASK_HIGH     0x70
#define DCC_ACC_STATE_BIT             0x08
#define DCC_ACC_OUTPUT_MASK           0x07

#define DCC_ACC_BASIC_LEN                3
#define DCC_ACC_BROADCAST_ADDRESS   0x003f

/* For CV programming / reading in service mode */
#define DCC_CV_DIRECT_MASK            0xf0
#define DCC_CV_DIRECT                 0x70
#define DCC_CV_DIRECT_MODE_MASK       0x0c
#define DCC_CV_DIRECT_RES             0x00
#define DCC_CV_DIRECT_VERIFY          0x04
#define DCC_CV_DIRECT_BIT_MAN         0x08
#define DCC_CV_DIRECT_WRITE           0x0c
#define DCC_CV_DIRECT_ADDR_HIGH_MASK  0x03

#define DCC_CV_DIRECT_BIT_RES_MASK    0xe0
#define DCC_CV_DIRECT_BIT_RES_VALUE   0xe0
#define DCC_CV_DIRECT_BIT_MODE_MASK   0x10
#define DCC_CV_DIRECT_BIT_MODE_VER    0x00
#define DCC_CV_DIRECT_BIT_MODE_WRITE  0x10
#define DCC_CV_DIRECT_BIT_BITS_MASK   0x07

#define DCC_CV_DIRECT_MODE_LEN           4

// Address only is basically deprecated and superceded
// by physical register addressing. In fact the packet
// for handling the address CV in physical addressing
// mode is identify. These are here for completeness.
#define DCC_CV_ADDRESS_ONLY_MASK      0xf7
#define DCC_CV_ADDRESS_ONLY           0x77
#define DCC_CV_ADDRESS_ONLY_MODE_MASK 0x08
#define DCC_CV_ADDRESS_ONLY_READ      0x00
#define DCC_CV_ADDRESS_ONLY_WRITE     0x08
#define DCC_CV_ADDRESS_ONLY_ADDR_BIT7 0x80
#define DCC_CV_ADDRESS_ONLY_ADDR_MASK 0x7f

#define DCC_CV_ADDRESS_MODE_LEN          3

#define DCC_CV_PHYSICAL_REG_MASK      0xf0
#define DCC_CV_PHYSICAL_REG           0x70
#define DCC_CV_PHYSICAL_REG_MODE_MASK 0x08
#define DCC_CV_PHYSICAL_REG_READ      0x00
#define DCC_CV_PHYSICAL_REG_WRITE     0x08
#define DCC_CV_PHYSICAL_REG_REG_MASK  0x07
#define DCC_CV_PHYSICAL_REG_ADDRESS   0x00

// 0x01 to 0x09 are manufacturer specific for accessory decoders

#define DCC_CV_PHYSICAL_REG_VERSION   0x06
#define DCC_CV_PHYSICAL_REG_MAN_ID    0x07

#define DCC_CV_PHYSICAL_REG_MODE_LEN     3

/* For CV programming / reading in operational mode */
#define DCC_CV_SHORT_FORM_BIT         0x10
#define DCC_CV_SHORT_MASK             0x0f
#define DCC_CV_SHORT_RESERVED         0x00
#define DCC_CV_SHORT_ACC              0x02
#define DCC_CV_SHORT_DEC              0x03
#define DCC_CV_SHORT_OTHER            0x09
#define DCC_CV_LONG_MODE_MASK         0x0c
#define DCC_CV_LONG_RESERVED          0x00
#define DCC_CV_LONG_VERIFY            0x04
#define DCC_CV_LONG_BIT_MAN           0x08
#define DCC_CV_LONG_WRITE             0x0c
#define DCC_CV_LONG_ADDR_HIGH_MASK    0x03

#ifdef __cplusplus
}
#endif

#endif