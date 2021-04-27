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

#ifndef __TWOWIRE_H
#define __TWOWIRE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** The error status (chip can never return this) */
#define TWOWIRE_ERROR 0xff

/**
 * \brief Initialise the two wire interface
 */
void twowire_init();

/**
 * \brief Write bytes to a device on the two wire interface
 *
 * This function writes one or more bytes to a device with the specified
 * address on the two wire bus. If bytes are only being written then set
 * send_stop to true. Otherwise, for example reading from registers in the
 * device, set it to false. 
 *
 * \param addr      the address
 * \param len       the length of the bytes array
 * \param bytes     the bytes to write
 * \param send_stop set true to send a stop after the write
 *
 * \return a status
 */
uint8_t twowire_writebytes(uint8_t addr, uint8_t len, uint8_t bytes[], bool send_stop);

/**
 * \brief Read bytes from a device on the two wire interface
 * 
 * This function reads one or more bytes from a device with the specified
 * address on the two wire bus. If a register, or registers, need to be set
 * before the read, use twowire_writebytes() to send the register values
 * but with send_stop set to false. The follow with a call to this function.
 *
 * \param addr  the address
 * \param len   the length of the bytes array
 * \param bytes the array in which to store the bytes
 *
 * \return a status
 */
uint8_t twowire_readbytes(uint8_t addr, uint8_t len, uint8_t bytes[]);

#ifdef __cplusplus
}
#endif

#endif
