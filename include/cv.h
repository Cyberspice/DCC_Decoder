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

#ifndef __CV_H
#define __CV_H

#include <stdint.h>

#define CV_REG_ADDRESS               1
#define CV_REG_ACC_ADDRESS         513
#define CV_REG_PAGE_REGISTER         6
#define CV_REG_ACC_PAGE_REGISTER   518
#define CV_REG_VERSION               7
#define CV_REG_MANUFACTURER_ID       8
#define CV_REG_ACC_MANUFACTURER_ID 520
#define CV_REG_ADDRESS_HIGH          9
#define CV_REG_ACC_ADDRESS_HIGH    521
#define CV_REG_CONFIGURATION        29

#define CV_VAL_ADDRESS               1
#define CV_VAL_VERSION               1
#define CV_VAL_MANUFACTURER_ID      13
#define CV_VAL_RESET                 8

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initialises the CV code
 */
void cv_init();

/**
 * \brief Resets the CV code back to factory defaults
 */
void cv_reset();

/**
 * \brief Read a CV value
 * 
 * \param cv the CV number
 * 
 * \returns the CV value
 */
uint8_t cv_read(uint16_t cv);

/**
 * \brief Write a CV value
 * 
 * \param cv the CV number
 * \param value the CV value
 * 
 * \returns true if the value was written
 */
bool cv_write(uint16_t cv, uint8_t value);

/**
 * \brief Returns the decoder address
 * 
 * \returns the decoder address
 */
uint16_t cv_get_dcc_addr();


#ifdef __cplusplus
}
#endif

#endif