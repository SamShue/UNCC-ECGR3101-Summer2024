#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#define GREEN_LED_PIN       BIT0
#define GREEN_LED_DIR_REG   P1DIR
#define GREEN_LED_OUT_REG   P1OUT

#define YELLOW_LED_PIN      (BIT1 | BIT3)
#define YELLOW_LED_DIR_REG  P2DIR
#define YELLOW_LED_OUT_REG  P2OUT

#define RED_LED_PIN         BIT6
#define RED_LED_DIR_REG     P1DIR
#define RED_LED_OUT_REG     P1OUT

typedef enum{
    GREEN, YELLOW, RED, MAX_STATES
} state_t;

void init();
void delay_ms(uint16_t ms);

static inline void setGreenLed(bool enable);
static inline void setYellowLed(bool enable);
static inline void setRedLed(bool enable);

state_t runGreenState(uint16_t* millis);
state_t runYellowState(uint16_t* millis);
state_t runRedState(uint16_t* millis);

state_t (*state_table[MAX_STATES])(uint16_t*) = {runGreenState, runYellowState, runRedState};

int main(void) {
    uint16_t millis = 0;
    state_t currentState = GREEN;
    state_t nextState = currentState;

    init();

    while(1) {
        /*
        switch(currentState){
        case GREEN:
            nextState = runGreenState(&millis);
            break;
        case YELLOW:
            nextState = runYellowState(&millis);
            break;
        case RED:
            nextState = runRedState(&millis);
            break;
        default:
            break;
        }
        */

        if(currentState < MAX_STATES){
            nextState = state_table[currentState](&millis);
        }

        currentState = nextState; // Update state
        delay_ms(1);
        millis++; // Update seconds
    }
}

state_t runGreenState(uint16_t* millis){
    setRedLed(false);
    setGreenLed(true);
    setYellowLed(false);

    if(*millis > 5000){
        *millis = 0;
        return YELLOW;
    }

    return GREEN;
}

state_t runYellowState(uint16_t* millis){
    setRedLed(false);
    setGreenLed(false);
    setYellowLed(true);

    if(*millis > 2000){
        *millis = 0;
        return RED;
    }

    return YELLOW;
}
state_t runRedState(uint16_t* millis){
    setRedLed(true);
    setGreenLed(false);
    setYellowLed(false);

    if(*millis > 5000){
        *millis = 0;
        return GREEN;
    }

    return RED;
}

void init(){
    // Stop the watchdog timer
    WDTCTL = 0x5A80;

    // LED configs
    GREEN_LED_DIR_REG |= GREEN_LED_PIN;
    YELLOW_LED_DIR_REG |= YELLOW_LED_PIN;
    RED_LED_DIR_REG |= RED_LED_PIN;
}

static inline void setGreenLed(bool enable){
    if(enable){
        GREEN_LED_OUT_REG |= GREEN_LED_PIN;
    } else {
        GREEN_LED_OUT_REG &= ~GREEN_LED_PIN;
    }
}

static inline void setYellowLed(bool enable){
    if(enable){
        YELLOW_LED_OUT_REG |= YELLOW_LED_PIN;
    } else {
        YELLOW_LED_OUT_REG &= ~YELLOW_LED_PIN;
    }
}

static inline void setRedLed(bool enable){
    if(enable){
        RED_LED_OUT_REG |= RED_LED_PIN;
    } else {
        RED_LED_OUT_REG &= ~RED_LED_PIN;
    }
}

void delay_ms(uint16_t ms){
    volatile uint16_t i;
    for(i = 0; i < ms; i++){
        __delay_cycles(1000);
    }
}

