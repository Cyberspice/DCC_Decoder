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
#include <avr/wdt.h>
#include <util/atomic.h>

#ifndef INC_ARDUINO_H
#include <util/delay.h>
#endif

#include "serialtx.h"
#include "mcp23008.h"
#include "heartbeat.h"
#include "dccrx.h"
#include "cv.h"

typedef enum {
    DECODER_STATE_UNKNOWN,
    DECODER_STATE_OPERATIONAL,
    DECODER_STATE_RESET,
    DECODER_STATE_SERVICE,
    DECODER_STATE_NOT_SERVICE
} DECODER_STATE;

static DECODER_STATE decoder_state = DECODER_STATE_RESET;

char str[6];

void print_packet() {
    for (uint8_t i = 0; i < packet_len; i++) {
        uint8_to_string(packet_data[i], str);
        send_serial_0_str(str);
        send_serial_0(' ');
    }
    send_serial_0('\n');
}

#define DEBUG_PRINT(str) \
    do { \
        send_serial_0_str(str); \
    } while (false)

#define DEBUG_PRINT_PACKET(str) \
    do { \
        send_serial_0_str(str); \
        print_packet(); \
    } while (false)

static void init_diag_led() {
    DDRB = DDRB | _BV(DDB2);
    PORTB = PORTB | _BV(PB2);
}

static void init_ack() {
    DDRD = DDRD | _BV(DDD5);
    PORTD = PORTD & ~_BV(PD5);
}

void setup() {
    // Disable the watchdog
    wdt_disable();

    init_builtin_led();
    init_heartbeat();
    init_serial_0();
    init_ack();
    cv_init();
    dccrx_init();
    init_diag_led();
    twowire_init();
    mcp23008_init(0x40);

    /* Configure sleep */
    set_sleep_mode(SLEEP_MODE_IDLE);

    /* A hello */
    send_serial_0_str("DCC Decoder 0.01\n");
    send_serial_0_str("----------------\n");
    dccrx_start();
}

/**
 * \brief Sends an acknowledge pulse
 * 
 * An acknowledge pulse is a 6ms load of at least 60mA on the input signal.
 * This is done using a 100 ohm resistor providing a 100mA load. At 15V this
 * dispels 1.5W so a decent wattage resistor needs to be used.
 */
inline void send_ack() {
    PORTD = PORTD  | _BV(PD5);

    // Incremented by timer 0 ever 1ms
    count_ms = 0;

    while (count_ms < 6) {
        // Sleep mode is okay here as we shouldn't be doing anything whilst
        // send an acknowledge pulse.
        sleep_mode();
    }

    PORTD = PORTD & ~_BV(PD5);
}

// handler functions return true if they processed the packet, or false if
// they ignored it.

/**
 * \brief Handle a reset packet
 *
 * This function resets the decoder state machine
 *
 * \returns true if the packet was processed
 */
inline bool handle_reset() {
    DEBUG_PRINT_PACKET("RST:");

    decoder_state = DECODER_STATE_RESET;

    // Ensure no one else is reliant on the count value
    count_ms = 0;

    return true;
}

/**
 * \brief Handle an idle packet
 *
 * \returns true if the packet was processed
 */
inline bool handle_idle() {
    return true;
}

/**
 * \brief Handle an accessory decoder packet
 *
 * \param dcc_addr the decoder address
 * \param addr_byte the address byte from the packet
 *
 * \returns true if the packet was processed
 */
inline bool handle_acc_packet(uint16_t dcc_addr, uint8_t addr_byte) {
    uint8_t inst_byte = packet_data[1];

    if ((inst_byte & DCC_ACC_BASIC_PACKET_BIT) == DCC_ACC_BASIC_PACKET_BIT) {
        // Check the length
        if (packet_len == DCC_ACC_BASIC_LEN) {
            // Upper address bits are ones complement
            uint16_t addr = (uint16_t)((inst_byte & DCC_ACC_ADDRESS_MASK_HIGH) ^ DCC_ACC_ADDRESS_MASK_HIGH);
            addr = (addr << 2) | (uint16_t)(addr_byte & DCC_ACC_ADDRESS_MASK_LOW);

            if ((addr == DCC_ACC_BROADCAST_ADDRESS) || (addr == dcc_addr)) {
                uint8_t output = inst_byte & DCC_ACC_OUTPUT_MASK;
                bool    state  = (inst_byte & DCC_ACC_STATE_BIT) == DCC_ACC_STATE_BIT;

                send_serial_0_str("ACC:");
                uint16_to_string(addr, str);
                send_serial_0_str(str);
                send_serial_0(' ');
                uint8_to_string(output, str);
                send_serial_0_str(str);
                send_serial_0(' ');
                send_serial_0_str(state ? "on" : "off");
                send_serial_0('\n');
            } else {
                send_serial_0_str("ACC:");
                uint16_to_string(addr, str);
                send_serial_0_str(str);
                send_serial_0('\n');
            }

            return true;
        }
    }

    return false;
}

/**
 * \brief Handle an operational mode packet
 *
 * \param dcc_addr the decoder address
 * \param addr_byte the address byte from the packet
 *
 * \returns true if the packet was processed
 */
inline bool handle_oper_packet(uint16_t dcc_addr, uint8_t addr_byte) {
    // Luckily the first byte of an accessory decoder packet must have the
    // top bit set which is never true for a service mode cv packet. So we
    // can tell when we have an accessory decoder packet easily

    if ((addr_byte & DCC_ADDRESS_ACCESSORY_MASK) == DCC_ADDRESS_ACCESSORY) {
        return handle_acc_packet(dcc_addr, addr_byte);
    } else {
        DEBUG_PRINT_PACKET("OPER:");
    }

    return false;
}

/**
 * \brief Handle a service mode packet
 *
 * \returns true if the packet was processed
 */
inline bool handle_service_packet(uint8_t addr_byte) {
    // The first byte of a service packet is no different to a the first
    // byte for a short form multi function decoder operational packet. We
    // have to use as much info as possible to determine the difference,
    // i.e. packet length, other bytes in the packet and decoder mode.

    uint8_t data = packet_data[2];
    if (packet_len == DCC_CV_DIRECT_MODE_LEN &&
        ((addr_byte & DCC_CV_DIRECT_MASK) == DCC_CV_DIRECT)) {
        uint16_t cv = ((uint16_t)(addr_byte & DCC_CV_DIRECT_ADDR_HIGH_MASK)) << 8;
        cv = cv | (uint16_t)(packet_data[1]);
        cv++;

        if ((addr_byte & DCC_CV_DIRECT_MODE_MASK) == DCC_CV_DIRECT_VERIFY) {
            if (data == cv_read(cv)) {
                send_ack();
            }

            return true;
        } else
        if ((addr_byte & DCC_CV_DIRECT_MODE_MASK) == DCC_CV_DIRECT_WRITE) {
            if (cv_write(cv, data)) {
                send_ack();
            }

            return true;
        } else
        if ((addr_byte & DCC_CV_DIRECT_MODE_MASK) == DCC_CV_DIRECT_BIT_MAN) {
            if ((data & DCC_CV_DIRECT_BIT_RES_MASK)  == DCC_CV_DIRECT_BIT_RES_VALUE) {
                uint8_t bit = data & DCC_CV_DIRECT_BIT_BITS_MASK;
                uint8_t value = cv_read(cv);

                if ((data & DCC_CV_DIRECT_BIT_MODE_MASK) == DCC_CV_DIRECT_BIT_MODE_VER) {

                    if ((0x05 & _BV(bit)) == _BV(bit)) {
                        send_ack();
                    } 
                } else {
                    value = value | _BV(bit);
                    if (cv_write(cv, value)) {
                        send_ack();
                    }
                }
            }

            return true;
        }
    } else
    if (packet_len == DCC_CV_PHYSICAL_REG_MODE_LEN &&
        ((addr_byte & DCC_CV_PHYSICAL_REG_MASK) == DCC_CV_PHYSICAL_REG)) {
        uint16_t reg = addr_byte & DCC_CV_PHYSICAL_REG;
        uint16_t page = cv_read(CV_REG_PAGE_REGISTER);
        uint16_t cv = 0;

        switch (reg) {
            case 0:
            case 1:
            case 2:
            case 3:
                cv = ((page - 1) * 4) + reg + 1;
                break;
            case 4:
                cv = CV_REG_CONFIGURATION;
                break;
            default:
                cv = ((uint16_t)reg) + 1;
                break;
        }

        if ((addr_byte & DCC_CV_DIRECT_MODE_MASK) == DCC_CV_DIRECT_VERIFY) {
            if (data == cv_read(cv)) {
                send_ack();
            }

            return true;
        } else
        if ((addr_byte & DCC_CV_DIRECT_MODE_MASK) == DCC_CV_DIRECT_WRITE) {
            if (cv_write(cv, data)) {
                send_ack();
            }

            return true;
        }
    }

    DEBUG_PRINT_PACKET("SER:");
    return false;
}

/**
 * /brief the main loop
 */
static void loop() {
    if (packet_len > 0) {
        if (dccrx_isvalid(packet_data, packet_len)) {
            uint8_t addr_byte = packet_data[0];

            switch (addr_byte) {
                case DCC_ADDRESS_BROADCAST:
                    DEBUG_PRINT("BROADCAST\n");
                    if (packet_len == DCC_BROADCAST_LEN) {
                        if (addr_byte == 0 && packet_data[1] == 0) {
                            handle_reset();
                        } else {
                            DEBUG_PRINT_PACKET("BCST:");
                        }
                    }
                    break;

                case DCC_ADDRESS_IDLE:
                    if (packet_len == DCC_IDLE_LEN) {
                        handle_idle();
                    }
                    break;

                default:
                    // Decoder state machine
                    switch (decoder_state) {
                        case DECODER_STATE_OPERATIONAL:
                            DEBUG_PRINT("OPERATIONAL:\n");
                            handle_oper_packet(cv_get_dcc_addr(), addr_byte);
                            break;
                        case DECODER_STATE_RESET:
                            // A Digital Decoder will enter service mode upon
                            // receipt of a valid service mode instruction
                            // packet immediately proceeded by a reset packet.
                            // S-9.2.3
                            DEBUG_PRINT("RESET:\n");
                            if (handle_service_packet(addr_byte)) {
                                decoder_state = DECODER_STATE_SERVICE;
                            } else {
                                decoder_state = DECODER_STATE_OPERATIONAL;
                                handle_oper_packet(cv_get_dcc_addr(), addr_byte);
                            }
                            break;
                        case DECODER_STATE_SERVICE:
                            // A Digital Decoder will exit service mode but
                            // not enter operations mode when any of the
                            // following conditions are true:
                            // 1) The Digital Decoder successfully receives
                            //    a nonservice mode instruction packet.
                            // 2) 20 milliseconds have occurred since the
                            //    last valid reset or service mode
                            //    instruction packet was received.
                            // S-9.2.3
                            DEBUG_PRINT("SERVICE:\n");
                            if (count_ms < 20) {
                                count_ms = 0;

                                if (handle_service_packet(addr_byte)) {
                                    break;
                                }
                                // else not a service packet
                            } else {
                                DEBUG_PRINT(">20ms\n");
                            }

                            decoder_state = DECODER_STATE_NOT_SERVICE;
                        case DECODER_STATE_NOT_SERVICE:
                            // Once a Digital Decoder has exited service mode,
                            // it will only re-enter operations mode upon
                            // receipt of a valid operations mode packet that
                            // is not identical to a service mode packet.
                            // S-9.2.3

                            // Top bit is always set for accessory decoder
                            // packets and NEVER set for service mode packets
                            // which makes this easy
                            DEBUG_PRINT("NOT SERVICE:");
                            if (handle_oper_packet(cv_get_dcc_addr(), addr_byte)) {
                                decoder_state = DECODER_STATE_OPERATIONAL;
                            }
                            break;
                        default:
                            // NOP
                            break;
                    }
                    break;
            }
        }
        dccrx_start();
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
