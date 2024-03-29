/**************** ECE2049 Lab 4 ******************/
/***************  2 October 2019 *****************/
/******  Benjamin Ward, Jonathan Ferreira ********/
/*************************************************/

#include <msp430.h>
#include <stdlib.h>
#include <stdio.h>
#include <inc/peripherals.h>
#include <inc/ADC.h>
#include <inc/DAC.h>
#include <main.h>

// States utilized in main program state machine
enum State
{
    WELCOME_SCREEN, WAIT_FOR_SELECTION, DC, SQUARE, SAWTOOTH, TRIANGLE, TEST
};

// Function prototypes
void drawWelcome();
void configButtons();
unsigned char getButtonState();
float readDAC(ADC*);

// Program entry point
void main(void)
{
    // Main loop state
    State state = WELCOME_SCREEN;

    uint8_t buttonPressed;

    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    // Set up and configure peripherals and I/O
//    initLeds();
    configDisplay();
    configButtons();
    configKeypad();

    ADC* adc = new ADC();
    DAC* dac = new DAC();
    uint32_t pot;

    _BIS_SR(GIE); // Global interrupt enable

    // Main loop
    while (1)
    {
        buttonPressed = getButtonState();
        pot = adc->getCurrentPot();

        if (buttonPressed & BIT0)
        {
            if(state == DC)
            {
                readDAC(adc);
            }
            state = DC;
        }
        else if (buttonPressed & BIT1)
        {
            state = SQUARE;
        }
        else if (buttonPressed & BIT2)
        {
            state = SAWTOOTH;
        }
        else if (buttonPressed & BIT3)
        {
            state = TRIANGLE;
        }

        switch (state)
        {
        case WELCOME_SCREEN:
            drawWelcome();
            state = WAIT_FOR_SELECTION;
            break;
        case WAIT_FOR_SELECTION:
            __no_operation();
            break;
        case DC:
            // Display DC value starting at 0v to VCC
            dac->SetDC(pot);
            break;
        case SQUARE:
            // Display square wave going from 0v to V_CC with 150Hz
            dac->SetSquareWave(pot);
            break;
        case SAWTOOTH:
            // Display sawtooth wave going from 0v to V_CC with 75Hz
            dac->SetSawtoothWave();
            break;
        case TRIANGLE:
            // Display triangle wave going from 0v to V_CC with 150Hz
            dac->SetTriangleWave(100 + (uint32_t)((pot * 400.0) / 4095.0)); // Frequency ranges from 100Hz at pot = 0 to 1kHz when pot = 4095
            break;
        }
    }
}

// Draw Welcome screen
void drawWelcome()
{
// *** Intro Screen ***
    Graphics_clearDisplay(&g_sContext);                // Clear the display

// Write some text to the display
    Graphics_drawStringCentered(&g_sContext, (uint8_t*) "MSP430",
    AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, (uint8_t*) "Function Gen",
    AUTO_STRING_LENGTH,
                                48, 35,
                                TRANSPARENT_TEXT);

    Graphics_drawString(&g_sContext, (uint8_t*) "B1: DC",
    AUTO_STRING_LENGTH,
                        10, 48, TRANSPARENT_TEXT);
    Graphics_drawString(&g_sContext, (uint8_t*) "B2: Square",
    AUTO_STRING_LENGTH,
                        10, 58, TRANSPARENT_TEXT);
    Graphics_drawString(&g_sContext, (uint8_t*) "B3: Sawtooth",
    AUTO_STRING_LENGTH,
                        10, 68, TRANSPARENT_TEXT);
    Graphics_drawString(&g_sContext, (uint8_t*) "B4: Triangle",
    AUTO_STRING_LENGTH,
                        10, 78, TRANSPARENT_TEXT);

// Draw a box around everything because it looks nice
    Graphics_Rectangle box = { .xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
    Graphics_drawRectangle(&g_sContext, &box);

// We are now done writing to the display.  However, if we stopped here, we would not
// see any changes on the actual LCD.  This is because we need to send our changes
// to the LCD, which then refreshes the display.
// Since this is a slow operation, it is best to refresh (or "flush") only after
// we are done drawing everything we need.
    Graphics_flushBuffer(&g_sContext);
}

// Configure lab board buttons
void configButtons()
{
// P7.0, P3.6, P2.2, P7.4
// Configure P2.2
    P2SEL &= ~(BIT2);    // Select pin for DI/O
    P2DIR &= ~(BIT2);    // Set pin as input
    P2REN |= (BIT2);    // Enable pull-up resistor
    P2OUT |= (BIT2);

// Configure P3.6
    P3SEL &= ~(BIT6);    // Select pin for DI/O
    P3DIR &= ~(BIT6);    // Set pin as input
    P3REN |= (BIT6);    // Enable pull-up resistor
    P3OUT |= (BIT6);

// Configure P7.0 and P7.4
    P7SEL &= ~(BIT4 | BIT0);    // Select pins for DI/O
    P7DIR &= ~(BIT4 | BIT0);    // Set pins as input
    P7REN |= (BIT4 | BIT0);    // Enable pull-up resistors
    P7OUT |= (BIT4 | BIT0);
}

// Get the state of the lab board buttons
uint8_t getButtonState()
{
    uint8_t ret = 0x00;
// P2.2
    if (~P2IN & BIT2)
        ret |= BIT2; // Button 2
// P3.6
    if (~P3IN & BIT6)
        ret |= BIT1;    // Button 1
// P7.0
    if (~P7IN & BIT0)
        ret |= BIT0;    // Button 0
// P7.4
    if (~P7IN & BIT4)
        ret |= BIT3;    // Button 3
    return ret;
}

float readDAC(ADC* adc) {
    uint16_t ADC_code = adc->getCurrentDAC();
    float DAC_output = ADC_code * 3.3 / 4095.0;
    return DAC_output;
}
