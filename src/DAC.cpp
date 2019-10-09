/*
 * DAC.cpp
 *
 *  Created on: Oct 6, 2019
 *      Author: blward
 */

#include <inc/DAC.h>
#include <inc/peripherals.h>

uint64_t DAC::clock = 0;

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

void DAC::SetOutput(uint16_t amplitude_code)
{
    if (amplitude_code == lastCode)
        return;
    lastCode = amplitude_code;

    DAC_PORT_CS_OUT &= ~DAC_PIN_CS;

    amplitude_code |= 0x3000; // Add control bits to DAC word
    uint8_t high_byte = (uint8_t) (amplitude_code & 0x00FF), low_byte =
            (uint8_t) ((amplitude_code & 0xFF00) >> 8);

    DAC_SPI_REG_TXBUF = high_byte;
    WaitForTransmission();
    DAC_SPI_REG_TXBUF = low_byte;
    WaitForTransmission();

    DAC_PORT_CS_OUT |= DAC_PIN_CS;
    ToggleLDAC();
}

void DAC::SetDC(uint32_t amplitude_code) {
    if (lastMode != DAC_DC)
    {
        ConfigTimer(0);
        lastMode = DAC_DC;
    }
    SetOutput(amplitude_code);
}

void DAC::SetSquareWave(uint32_t amplitude_code)
{
    if (lastMode != DAC_SQUARE)
    {
        ConfigTimer(150);
        lastMode = DAC_SQUARE;
    }

    SetOutput(clock % 2 == 0 ? amplitude_code : 0);
}

void DAC::SetSawtoothWave()
{
    if (lastMode != DAC_SAWTOOTH)
    {
        ConfigTimer(75 * stepCount);
        lastMode = DAC_SAWTOOTH;
    }

    // TODO: Watch out for bad integer division here
    SetOutput((clock % stepCount) * maxCode / stepCount);
}

void DAC::SetTriangleWave(uint32_t frequency)
{
    if (lastMode != DAC_TRIANGLE)
    {
        ConfigTimer(frequency * stepCount);
        lastMode = DAC_TRIANGLE;
    }

    // TODO: Watch out for bad integer division here
    // Hopefully I've "solved" the problem by using all kinds of 32 bit integers
    if (clock % (stepCount * 2) > stepCount)
        SetOutput(((clock % stepCount) * (maxCode / stepCount)));
    else
        SetOutput(((stepCount - (clock % stepCount)) * (maxCode / stepCount)));
}

// Configure Timer A2, used to schedule changes in DAC output for square/sawtooth/triangle modes
void DAC::ConfigTimer(uint32_t frequency) {
    // If we're not using the timer, disable interrupts
    if(!frequency) {
        // We don't care how the timer is configured; just disable interrupts so we don't waste CPU time
        TA2CCTL0 &= ~CCIE; // Disable interrupt
        return;
    }

    TA2CTL = TASSEL_2 + ID_0 + MC_1; // SMCLK, no divider, continuous mode
    TA2CCR0 = (uint16_t)(1048676 / frequency); // SMCLK frequency / input frequency = timer ticks
    TA2CCTL0 = CCIE; // TA2CCR0 interrupt enabled
}

#pragma vector=TIMER2_A0_VECTOR
__interrupt void DAC::TimerISR() {
    clock++;
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
