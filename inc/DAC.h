/*
 * DAC.h
 *
 *  Created on: Oct 6, 2019
 *      Author: blward
 */

#ifndef INC_DAC_H_
#define INC_DAC_H_

#include <stdint.h>

class DAC
{
public:
    DAC();
    virtual ~DAC();
    void SetOutput(uint16_t code);
private:
    void WaitForTransmission(void);
    void ToggleLDAC(void);
    uint16_t lastCode;
};

#endif /* INC_DAC_H_ */
