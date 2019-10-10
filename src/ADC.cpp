/*
 * TemperatureSensor.cpp
 *
 *  Created on: Sep 25, 2019
 *      Author: blward
 */

#include <inc/ADC.h>
#include <main.h>
#include <msp430.h>
#include <numeric>

// Global variables
uint16_t ADC::rawPotReading;
uint16_t ADC::rawDACReading;

ADC::ADC()
{
    REFCTL0 &= ~REFMSTR; // Reset REFMSTR to hand over control of internal reference

    // Potentiometer
    ADC12CTL0 = ADC12SHT0_9 | ADC12REFON | ADC12ON | ADC12MSC; // Start ADC with 1.5V internal reference
    ADC12CTL1 = ADC12SHP | ADC12CONSEQ_1 | ADC12CSTARTADD_0;

    // Using ADC12MEM1 for conversion result, so enable interrupt for MEM1
    ADC12IE = (BIT1);

    ADC12MCTL0 = ADC12SREF_0 | ADC12INCH_0; // Use 3.3V ref (VCC)
    ADC12MCTL1 = ADC12SREF_0 | ADC12INCH_1 | ADC12EOS;
    P6SEL |= (BIT0 | BIT1); // Set P6.0 and P6.1 to function mode to enable use of A0 and A1

    // Configure Timer B0, used to schedule ADC readings independent of the DAC output (Timer A2)
    TB0CTL = TASSEL_1 + ID_0 + MC_1;
    TB0CCR0 = 3277; // = ~10 Hz
    TB0CCTL0 = CCIE; // TB0CCR0 interrupt enabled
}

ADC::~ADC()
{
    // Not necessary, ADC instance will not be deconstructed
}

// Get current potentiometer reading
uint16_t ADC::getCurrentPot() {
    return rawPotReading;
}

// Get current potentiometer reading
uint16_t ADC::getCurrentDAC() {
    return rawDACReading;
}

// Configure the Timer B0 ISR
#pragma vector=TIMER0_B0_VECTOR
__interrupt void ADC::TIMERISR()
{
    ADC12CTL0 |= (ADC12SC | ADC12ENC); // Start conversion
}

// Configure the potentiometer ISR
#pragma vector=ADC12_VECTOR
__interrupt void ADC::ADC12ISR() {
    // Move results to static
    rawPotReading = ADC12MEM0 & 0x0FFF;
    rawDACReading = ADC12MEM1 & 0x0FFF;
}

