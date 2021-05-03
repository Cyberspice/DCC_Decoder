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

#include <avr/eeprom.h>

#include "serialtx.h"
#include "cv.h"

#define EEPROM_ADDRESS_GUARD                0
#define EEPROM_SIZE_OF_GUARD                4
#define EEPROM_ADDRESS_CV_ADDRESS           5
#define EEPROM_ADDRESS_CV_ADDRESS_HIGH      6
#define EEPROM_ADDRESS_CV_PAGE_REGISTER     7

char cvstr[6];

void cv_init() {
    char guard[EEPROM_SIZE_OF_GUARD];

    eeprom_busy_wait();
    eeprom_read_block(guard, EEPROM_ADDRESS_GUARD, sizeof(guard));

    // If the EEPROM isn't initialised, factory reset
    if (guard[0] != 'M' || guard[1] != 'E' || guard[2] != 'L' || guard[3] != '!') {
        cv_reset();
    }
}

void cv_reset() {
    char guard[EEPROM_SIZE_OF_GUARD] = { 'M', 'E', 'L', '!' };

    send_serial_0_str("CV RESET\n");
    eeprom_busy_wait();
    eeprom_write_block(guard, EEPROM_ADDRESS_GUARD, sizeof(guard));
    eeprom_write_byte((uint8_t *)EEPROM_ADDRESS_CV_ADDRESS, CV_VAL_ADDRESS);
    eeprom_write_byte((uint8_t *)EEPROM_ADDRESS_CV_ADDRESS_HIGH, 0);
    eeprom_write_byte((uint8_t *)EEPROM_ADDRESS_CV_PAGE_REGISTER, 7);
}

uint8_t cv_read(uint16_t cv) {
    uint8_t value = 0;

    eeprom_busy_wait();

    switch (cv) {
        case CV_REG_ADDRESS:
        case CV_REG_ACC_ADDRESS:
            value = eeprom_read_byte((uint8_t *)EEPROM_ADDRESS_CV_ADDRESS);
            break;
        case CV_REG_VERSION:
            value = CV_VAL_VERSION;
            break;
        case CV_REG_MANUFACTURER_ID:
        case CV_REG_ACC_MANUFACTURER_ID:
            value = CV_VAL_MANUFACTURER_ID;
            break;
        case CV_REG_ADDRESS_HIGH:
        case CV_REG_ACC_ADDRESS_HIGH:
            value = eeprom_read_byte((uint8_t *)EEPROM_ADDRESS_CV_ADDRESS_HIGH);
            break;
        default:
            break;
    }

    send_serial_0_str("CV READ: ");
    uint16_to_string(cv, cvstr);
    send_serial_0_str(cvstr);
    send_serial_0(' ');
    uint8_to_string(value, cvstr);
    send_serial_0_str(cvstr);
    send_serial_0('\n');

    return value;
}

bool cv_write(uint16_t cv, uint8_t value) {
    eeprom_busy_wait();

    send_serial_0_str("CV WRITE: ");
    uint16_to_string(cv, cvstr);
    send_serial_0_str(cvstr);
    send_serial_0(' ');
    uint8_to_string(value, cvstr);
    send_serial_0_str(cvstr);
    send_serial_0('\n');

    switch (cv) {
        case CV_REG_ADDRESS:
        case CV_REG_ACC_ADDRESS:
        {
            uint8_t high = eeprom_read_byte((uint8_t *)EEPROM_ADDRESS_CV_ADDRESS_HIGH);
            if (high == 0 && value == 0) {
                // address 0 is not allowed
                break;
            }

            eeprom_write_byte((uint8_t *)EEPROM_ADDRESS_CV_ADDRESS, value);
            return true;
        }
        case CV_REG_MANUFACTURER_ID:
        case CV_REG_ACC_MANUFACTURER_ID:
        {
            if (value == CV_VAL_RESET) {
                cv_reset();
            }
            return true;
        }
        case CV_REG_ADDRESS_HIGH:
        case CV_REG_ACC_ADDRESS_HIGH:
        {
            if (value <  8) {
                eeprom_write_byte((uint8_t *)EEPROM_ADDRESS_CV_ADDRESS_HIGH, value);
                return true;
            }
        }
        case CV_REG_CONFIGURATION:
            return true;
        default:
            break;
    }

    return false;
}

uint16_t cv_get_dcc_addr() {
    return (uint16_t)cv_read(CV_REG_ACC_ADDRESS);
}
