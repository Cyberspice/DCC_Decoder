
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "mcp23008.h"

/** Local copy of the output latch */
static uint8_t olat = 0;

uint8_t mcp23008_write_reg(uint8_t addr, uint8_t reg, uint8_t val) {
    static uint8_t buf[2];
    buf[0] = reg;
    buf[1] = val;

    return twowire_writebytes(addr, 2, buf, true);
}

uint8_t mcp23008_read_reg(uint8_t addr, uint8_t reg, uint8_t * pval) {
    uint8_t status = twowire_writebytes(addr, 1, &reg, false);
    if (status != TWOWIRE_ERROR) {
        status = twowire_readbytes(addr, 1, pval);
    }

    return status;
}

void mcp23008_init(uint8_t addr) {
    // PD3 as input with no pullup
    DDRD = DDRD & ~_BV(MCP23008_INT);
    PORTD = PORTD & ~_BV(MCP23008_INT);

    // PB1 as output set high (reset while initing)
    DDRB = DDRB | _BV(MCP23008_RST);
    PORTB = PORTB & ~_BV(MCP23008_RST);
    _delay_ms(10);
    PORTB = PORTB | _BV(MCP23008_RST);

    // Set all pins as outputs
    mcp23008_write_reg(addr, MCP23008_IODIR, 0);

    // Clear the output latch copy
    olat = 0;
}

void mcp23008_set_olat(uint8_t addr, uint8_t bits) {
    olat = olat | bits;
    mcp23008_write_reg(addr, MCP23008_OLAT, olat);
}

void mcp23008_clear_olat(uint8_t addr, uint8_t bits) {
    olat = olat & ~bits;
    mcp23008_write_reg(addr, MCP23008_OLAT, olat);
}
