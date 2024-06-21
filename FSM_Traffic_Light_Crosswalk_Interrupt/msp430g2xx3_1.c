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
    GREEN, YELLOW, RED, CROSSWALK, MAX_STATES
} state_t;

typedef struct{
    uint16_t seconds;
    bool buttonPressed;
} inputs_t;

void init();
void delay_s(uint16_t seconds);

state_t runRedState(inputs_t*);
state_t runYellowState(inputs_t*);
state_t runGreenState(inputs_t*);
state_t runCrossWalkState(inputs_t*);

static inline void setGreenLed(bool enable);
static inline void setYellowLed(bool enable);
static inline void setRedLed(bool enable);
static inline bool isButtonPressed();

state_t (*state_table[MAX_STATES])(inputs_t*) = {runGreenState, runYellowState, runRedState, runCrossWalkState};

inputs_t inputs;

int main(void) {
    state_t currentState = GREEN;
    state_t nextState = currentState;

    inputs.seconds = 0;
    inputs.buttonPressed = false;

    init();

    while(1) {
        if(currentState < MAX_STATES){
            nextState = state_table[currentState](&inputs);
        }

        // Update state
        currentState = nextState;

        // Update seconds
        delay_s(1);
        inputs.seconds++;
    }
}

void init(){
    // Stop the watchdog timer
    WDTCTL = 0x5A80;

    // LED configs
    GREEN_LED_DIR_REG |= GREEN_LED_PIN;
    YELLOW_LED_DIR_REG |= YELLOW_LED_PIN;
    RED_LED_DIR_REG |= RED_LED_PIN;

    // Button config
    P1DIR &= ~BIT3;
    P1REN |= BIT3;
    P1OUT |= BIT3;

    P1IES |= BIT3;  // high to low trigger
    P1IE |= BIT3;   // enable interrupt
    __enable_interrupt();   // global enable
}

state_t runRedState(inputs_t* inputs){
    state_t nextState = RED;

    // set output
    setYellowLed(false);
    setGreenLed(false);
    setRedLed(true);

    // next state logic
    if(inputs->seconds > 5){
        inputs->seconds = 0;
        nextState = GREEN;
    }

    return nextState;
}

state_t runYellowState(inputs_t* inputs){
    state_t nextState = YELLOW;

    // set output
    setRedLed(false);
    setGreenLed(false);
    setYellowLed(true);
    // next state logic
    if(inputs->seconds > 2){
        inputs->seconds = 0;
        if(inputs->buttonPressed){
            nextState = CROSSWALK;
        } else {
            nextState = RED;
        }
    }

    return nextState;
}

state_t runGreenState(inputs_t* inputs){
    state_t nextState = GREEN;

    // set output
    setGreenLed(true);
    setRedLed(false);
    setYellowLed(false);
    // next state logic
    if(inputs->seconds > 5){
        inputs->seconds = 0;
        nextState = YELLOW;
    }

    return nextState;
}

state_t runCrossWalkState(inputs_t* inputs){
    state_t nextState = CROSSWALK;
    static bool yellowOn = false;

    // set output
    yellowOn = !yellowOn;
    setGreenLed(false);
    setRedLed(false);
    setYellowLed(yellowOn);

    // next state logic
    if(inputs->seconds > 6){
        inputs->seconds = 0;
        inputs->buttonPressed = false;
        nextState = RED;
    }

    return nextState;
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

void delay_s(uint16_t seconds){
    volatile uint16_t i;
    for(i = 0; i < seconds; i++){
        __delay_cycles(1000000);
    }
}

static inline bool isButtonPressed(){
    return (P1IN & BIT3) == 0x00;
}

#pragma vector=PORT1_VECTOR
__interrupt void Port1ISR(){

    if(P1IFG & BIT3){
        inputs.buttonPressed = true;

        P1IFG &= ~BIT3;
    }
}
