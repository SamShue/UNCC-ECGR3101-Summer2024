#ifndef HAL_H_
#define HAL_H_

#include <stdbool.h>

#define GREEN_LED_PDIR      P2DIR
#define GREEN_LED_POUT      P2OUT
#define GREEN_LED_BIT       BIT3

#define RED_LED_PDIR        P1DIR
#define RED_LED_POUT        P1OUT
#define RED_LED_BIT         BIT6

#define BUTTON_PDIR         P1DIR
#define BUTTON_PIN          P1IN
#define BUTTON_POUT         P1OUT
#define BUTTON_PREN         P1REN
#define BUTTON_BIT          BIT3

static inline void setGreenLed(bool enable){
    if(enable){
        GREEN_LED_POUT |= GREEN_LED_BIT;
    } else {
        GREEN_LED_POUT &= ~GREEN_LED_BIT;
    }
}

static inline void setRedLed(bool enable){
    if(enable){
        RED_LED_POUT |= RED_LED_BIT;
    } else {
        RED_LED_POUT &= ~RED_LED_BIT;
    }
}

static inline bool isButtonPressed(){
    return (BUTTON_PIN & BUTTON_BIT) == 0x00;
}

static inline bool isButtonPressed_Debounced(){
    if(isButtonPressed()){
        __delay_cycles(10000);
        if(isButtonPressed()){
            return true;
        }
    }

    return false;
}

static inline void init(){
    WDTCTL = 0x5A80; // Stop the watchdog timer

    GREEN_LED_PDIR |= GREEN_LED_BIT;
    RED_LED_PDIR |= RED_LED_BIT;
    BUTTON_PDIR &= ~BUTTON_BIT;
    BUTTON_POUT |= BUTTON_BIT;
    BUTTON_PREN |= BUTTON_BIT;
}

#endif
