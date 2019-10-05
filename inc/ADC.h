/*
 * TemperatureSensor.h
 *
 *  Created on: Sep 25, 2019
 *      Author: blward
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include <stdint.h>

class ADC
{
public:
    ADC();
    virtual ~ADC();
    float getCurrentTempF();
    float getCurrentTempC();
    uint32_t getCurrentPot();
private:
    static __interrupt void ADC12ISR();
    static __interrupt void TIMERA2ISR();
    static uint32_t rawTempReadings[10];
    static uint32_t averagedTempReadings[30];
    static uint32_t rawPotReading;
    static uint32_t readingIndex;
    uint16_t tempCal30C;
    uint16_t tempCal85C;
    float degCPerBit;
};

class DAC
{

};


#endif /* INC_ADC_H_ */
