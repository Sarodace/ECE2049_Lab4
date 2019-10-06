/*
 * DAC.cpp
 *
 *  Created on: Oct 6, 2019
 *      Author: blward
 */

#include <inc/DAC.h>
#include <inc/peripherals.h>

DAC::DAC()
{
    setupSPI_DAC();

    DAC_PORT_LDAC_SEL &= ~DAC_PIN_LDAC;
    DAC_PORT_LDAC_DIR |= DAC_PIN_LDAC;
    DAC_PORT_LDAC_OUT |= DAC_PIN_LDAC;

    DAC_PORT_CS_SEL &= ~DAC_PIN_CS;
    DAC_PORT_CS_DIR |= DAC_PIN_CS;
    DAC_PORT_CS_OUT |= DAC_PIN_CS;
}

DAC::~DAC()
{
    // TODO Auto-generated destructor stub
}

void DAC::SetOutput(uint16_t code)
{
    if(code == lastCode) return;
    lastCode = code;

    DAC_PORT_CS_OUT &= ~DAC_PIN_CS;

    code |= 0x3000; // Add control bits to DAC word
    uint8_t high_byte = (uint8_t) (code & 0x00FF), low_byte = (uint8_t) ((code
            & 0xFF00) >> 8);

    DAC_SPI_REG_TXBUF = high_byte;
    WaitForTransmission();
    DAC_SPI_REG_TXBUF = low_byte;
    WaitForTransmission();

    DAC_PORT_CS_OUT |= DAC_PIN_CS;
    ToggleLDAC();
}

void DAC::WaitForTransmission(void)
{
    while (!(DAC_SPI_REG_IFG & UCTXIFG))
    {
        __no_operation();
    }
}

void DAC::ToggleLDAC(void)
{
    DAC_PORT_LDAC_OUT &= DAC_PIN_LDAC;
    DAC_PORT_LDAC_OUT &= DAC_PIN_LDAC;
}
