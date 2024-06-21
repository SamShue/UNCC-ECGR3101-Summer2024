#include <msp430.h>
#include <stdint.h>
#include "ws2812.h"

void gradualFill(u_int n, u_char r, u_char g, u_char b);
void setTrainPosition(uint16_t pos);

int main(void) {
    WDTCTL = WDTPW + WDTHOLD;  // Stop WDT
    if (CALBC1_16MHZ==0xFF)    // If calibration constant erased
    {
        while(1);              // do not load, trap CPU!!
    }

    // configure clock to 16 MHz
    BCSCTL1 = CALBC1_16MHZ;    // DCO = 16 MHz
    DCOCTL = CALDCO_16MHZ;

    // initialize LED strip
    initStrip();

    volatile uint16_t pos = 0;
    while (1) {
        setTrainPosition(pos);
        pos = (pos + 1) % NUM_LEDS;
        __delay_cycles(1000000);
    }
}

void gradualFill(u_int n, u_char r, u_char g, u_char b){
    int i;
    for (i = 0; i < n; i++){        // n is number of LEDs
        setLEDColor(i, r, g, b);
        showStrip();
        _delay_cycles(1000000);       // lazy delay
    }
}

void setTrainPosition(uint16_t pos){
    static const uint16_t TRAIN_LEN = 3;
    static const uint8_t BRIGHTNESS = 0xFF/6;
    clearStrip();
    uint16_t i;
    for(i = 0; i < TRAIN_LEN; i++)
        setLEDColor((pos + i) % NUM_LEDS, BRIGHTNESS, BRIGHTNESS, BRIGHTNESS);
    showStrip();
}
