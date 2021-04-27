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

#ifndef __MCP23008_H
#define __MCP23008_H

#include "twowire.h"

#define MCP23008_IODIR        0x00
#define MCP23008_IPOL         0x01
#define MCP23008_GPINTEN      0x02
#define MCP23008_DEFVAL       0x03
#define MCP23008_INTCON       0x04
#define MCP23008_IOCON        0x05
#define MCP23008_GPPU         0x06
#define MCP23008_INTF         0x07
#define MCP23008_INTCAP       0x08
#define MCP23008_GPIO         0x09
#define MCP23008_OLAT         0x0a

#define MCP23008_GP0          0
#define MCP23008_GP1          1
#define MCP23008_GP2          2
#define MCP23008_GP3          3
#define MCP23008_GP4          4
#define MCP23008_GP5          5
#define MCP23008_GP6          6
#define MCP23008_GP7          7

#define MCP23008_IOCON_SEQOP  0x20
#define MCP23008_IOCON_DISSLW 0x10
#define MCP23008_IOCON_ODR    0x04
#define MCP23008_IOCON_INTPOL 0x02

#define MCP23008_RST          PB1
#define MCP23008_INT          PD3

#define MCP23008_MAX_BYTES    (MCP23008_OLAT + 1)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Write the value of an MCP23008 register.
 *
 * \param addr the address of the device
 * \param reg  the register
 * \param val  the byte value
 *
 * \return the status
 */
uint8_t mcp23008_write_reg(uint8_t addr, uint8_t reg, uint8_t val);

/**
 * \brief Read the value of an MCP23008 register.
 *
 * \param addr the address of the device
 * \param reg  the register
 * \param pval a pointer to a variable in which to place the value
 *
 * \return the status
 */
uint8_t mcp23008_read_reg(uint8_t addr, uint8_t reg, uint8_t * pval);

/**
 * \brief Initialise the MCP23008
 * 
 * \param addr the address of the device
 */
void mcp23008_init(uint8_t addr);

/**
 * \brief Sets GPIOs in the OLAT register
 *
 * \param addr the address of the device
 * \param bits the bits to set
 */
void mcp23008_set_olat(uint8_t addr, uint8_t bits);

/**
 * \brief Clears GPIOs in the OLAT register
 *
 * \param addr the address of the device
 * \param bits the bits to clear
 */
void mcp23008_clear_olat(uint8_t addr, uint8_t bits);

#ifdef __cplusplus
}
#endif

#endif
