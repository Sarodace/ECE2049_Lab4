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
uint32_t ADC::rawTempReadings[10];
uint32_t ADC::averagedTempReadings[30];
uint32_t ADC::rawPotReading;
uint32_t ADC::readingIndex;

ADC::ADC()
{
    // Using the chip-specific calibration data stored in device registers, calculate
    // the ADC reading to degrees Celsius conversion
    tempCal30C = *((unsigned int *)0x1A1A);
    tempCal85C = *((unsigned int *)0x1A1C);
    this->degCPerBit = (85.0F - 30.0F)/(float)(tempCal85C - tempCal30C);

    REFCTL0 &= ~REFMSTR; // Reset REFMSTR to hand over control of internal reference

    // Potentiometer
    ADC12CTL0 = ADC12SHT0_9 | ADC12REFON | ADC12ON | ADC12MSC; // Start ADC with 1.5V internal reference

    // Temperature Sensor
    ADC12CTL1 = ADC12SHP | ADC12CONSEQ_1 | ADC12CSTARTADD_0;

    // Using ADC12MEM1 for conversion result, so enable interrupt for MEM1
    ADC12IE = (BIT0 | BIT1);

    ADC12MCTL0 = ADC12SREF_0 | ADC12INCH_0; // Use 3.3V ref (VCC)
    ADC12MCTL1 = ADC12SREF_1 | ADC12INCH_10 | ADC12EOS; // Use 1.5V ref
    P6SEL |= BIT0; // Set P6.0 to function mode to enable use of A0

    // Configure Timer A2
    TA2CTL = TASSEL_1 + ID_3 + MC_1;
    TA2CCR0 = 4095; // = ~1 second
    TA2CCTL0 = CCIE; // TA2CCR0 interrupt enabled

    for (int i = 0; i < 5; i++) {
        __delay_cycles(10000);
        ADC12CTL0 |= (ADC12SC | ADC12ENC); // Start conversion

        // Poll busy bit waiting for the first conversion to complete
        while (ADC12CTL1 & ADC12BUSY)
            __no_operation();
    }

    // Collect temperature readings
    for(int i = 0; i < 10; i++)
        rawTempReadings[i] = ADC12MEM1 & 0x0FFF;

    // Average the temperature readings
    for(int i = 0; i < 30; i++)
        averagedTempReadings[i] = ADC12MEM1 & 0x0FFF;

    _BIS_SR(GIE); // Global interrupt enable
}

ADC::~ADC()
{
    // Not necessary, ADC instance will not be deconstructed
}

// Get current temperature in Farenheit
float ADC::getCurrentTempF()
{
    return getCurrentTempC() * 9.0F/5.0F + 32.0F;
}

// Get current temperature in Celsius
float ADC::getCurrentTempC()
{
    return (float)(((int32_t)averagedTempReadings[readingIndex % 30]) - tempCal30C) * degCPerBit + 30.0F;
}

// Get current potentiometer reading
uint32_t ADC::getCurrentPot() {
    return rawPotReading;
}

// Configure the Timer A2 ISR
#pragma vector=TIMER2_A0_VECTOR
__interrupt void ADC::TIMERA2ISR()
{
//    globalCounter++;
//    globalClock++;
    ADC12CTL0 |= (ADC12SC | ADC12ENC); // Start conversion
}

// Configure the temperature and potentiometer ISR
#pragma vector=ADC12_VECTOR
__interrupt void ADC::ADC12ISR() {
    // Move results to static
    rawPotReading = ADC12MEM0 & 0x0FFF;
    rawTempReadings[readingIndex % 10] = ADC12MEM1 & 0x0FFF;

    int i, sum  = 0;
    for(i = 0; i < 10; i++)
    {
        sum += rawTempReadings[i];
    }

    averagedTempReadings[readingIndex % 30] = sum / 10;

    readingIndex++;
}

