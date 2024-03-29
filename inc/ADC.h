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
    uint16_t getCurrentPot();
    uint16_t getCurrentDAC();
private:
    static __interrupt void ADC12ISR();
    static __interrupt void TIMERISR();
    static uint16_t rawPotReading;
    static uint16_t rawDACReading;
};

#endif /* INC_ADC_H_ */
