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
    DAC_NONE,
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
    static void SetOutput(uint16_t amplitude_code);
    static void WaitForTransmission(void);
    static void ToggleLDAC(void);
    void ConfigTimer(uint32_t frequency);
    static uint16_t lastCode;
    static DACMode lastMode;

    static const uint32_t maxCode = 4095;
    static const uint32_t stepCount = 20; // TODO: Lower this if it's too many steps

    static void TimerISR(void);
    static uint64_t clock;
};

#endif /* INC_DAC_H_ */
