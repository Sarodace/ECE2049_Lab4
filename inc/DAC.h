/*
 * DAC.h
 *
 *  Created on: Oct 6, 2019
 *      Author: blward
 */

#ifndef INC_DAC_H_
#define INC_DAC_H_

#include <stdint.h>

enum DACMode {
    DAC_DC,
    DAC_SQUARE,
    DAC_SAWTOOTH,
    DAC_TRIANGLE
};

class DAC
{
public:
    DAC();
    virtual ~DAC();
    void SetDC(uint32_t amplitude_code);
    void SetSquareWave(uint32_t amplitude_code);
    void SetSawtoothWave();
    void SetTriangleWave(uint32_t frequency);
private:
    void SetOutput(uint16_t amplitude_code);
    void WaitForTransmission(void);
    void ToggleLDAC(void);
    void ConfigTimer(uint32_t frequency);
    uint16_t lastCode;
    DACMode lastMode;

    const uint16_t maxCode = 4095;
    const uint16_t stepCount = 100; // TODO: Lower this if it's too many steps

    static void TimerISR(void);
    static uint64_t clock;
};

#endif /* INC_DAC_H_ */
