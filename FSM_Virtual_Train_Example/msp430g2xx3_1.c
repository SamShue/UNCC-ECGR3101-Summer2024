#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include "ws2812.h"

void init();
void setTrainPosition(uint16_t pos);

typedef enum {STOP, FORWARD, REVERSE, NUM_STATES} state_t;
typedef enum {FORWARDS, BACKWARDS} direction_t;

typedef struct {
    direction_t direction;
    bool isLongPress;
    bool isShortPress;
    uint16_t position;
} inputs_t;

state_t runStopState(inputs_t*);
state_t runForwardState(inputs_t*);
state_t runReverseState(inputs_t*);

inputs_t inputs;

int main(void) {
    init();

    setTrainPosition(inputs.position);

    state_t currentState = STOP;
    state_t nextState = currentState;

    inputs.direction = FORWARDS;
    inputs.isLongPress = false;
    inputs.isShortPress = false;
    inputs.position = 0;

    volatile uint16_t pos = 0;
    while (1) {
        switch(currentState){
        case STOP:
            nextState = runStopState(&inputs);
            break;
        case FORWARD:
            nextState = runForwardState(&inputs);
            break;
        case REVERSE:
            nextState = runReverseState(&inputs);
            break;
        default:
            break;
        }

        currentState = nextState;
    }
}

void init(){
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

    // Button config
    P1DIR &= ~BIT3;
    P1REN |= BIT3;
    P1OUT |= BIT3;

    P1IES |= BIT3;  // high to low trigger
    P1IE |= BIT3;   // enable interrupt
    __enable_interrupt();   // global enable
}

state_t runStopState(inputs_t* inputs){
    // Do nothing
    if(inputs->isLongPress){
        inputs->isLongPress = false;
        inputs->direction = !inputs->direction; // Flip the direction
    }

    if(inputs->isShortPress){
        inputs->isShortPress = false;
        if(inputs->direction == FORWARDS){
            return FORWARD;
        } else {
            return REVERSE;
        }
    }
    return STOP;
}

state_t runForwardState(inputs_t* inputs){
    setTrainPosition(inputs->position);
    inputs->position = (inputs->position + 1) % NUM_LEDS;
    __delay_cycles(1000000);

    if(inputs->isShortPress){
        inputs->isShortPress = false;
        return STOP;
    }

    if(inputs->isLongPress){
        inputs->isLongPress = false;
        inputs->direction = BACKWARDS;
        return REVERSE;
    }
    return FORWARD;
}

state_t runReverseState(inputs_t* inputs){
    setTrainPosition(inputs->position);
    inputs->position = (inputs->position - 1) % NUM_LEDS;
    __delay_cycles(1000000);

    if(inputs->isShortPress){
        inputs->isShortPress = false;
        return STOP;
    }

    if(inputs->isLongPress){
        inputs->isLongPress = false;
        inputs->direction = FORWARDS;
        return FORWARD;
    }

    return REVERSE;
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

#pragma vector=PORT1_VECTOR
__interrupt void Port1ISR(){

    if(P1IFG & BIT3){
        __delay_cycles(16000000);   // Shouldn't do this!

        if(P1IN & BIT3){ // button is no longer pressed
            inputs.isShortPress = true;
        }

        inputs.isLongPress = true;
        P1IFG &= ~BIT3;
    }
}
