#include <stdbool.h>
#include <stdint.h>
#include <msp430.h>

#define BUTTON_PDIR         P1DIR
#define BUTTON_POUT         P1OUT
#define BUTTON_PIN          P1IN
#define BUTTON_PREN         P1REN
#define BUTTON_BIT          BIT3


static const uint16_t NUM_LEDS = 7;

typedef enum {waitingToStart, leftToRight, rightToLeft, losing, winning} state_t;
typedef struct {
    bool buttonPressed;
    uint16_t buttonTester;
} inputs_t;

void init();
void delay_ms(uint16_t milliseconds);
static inline void setAllLEDs(bool enable);
static inline void setBlueLED(bool enable);
static inline bool isButtonPressed();
static inline void runRightSweep(uint16_t* LED_BITS, bool enable);
static inline void runLeftSweep(uint16_t* LED_BITS, bool enable);
state_t runWaitingState(inputs_t* inputs);
state_t runLeftToRightState(inputs_t* inputs, uint16_t* LED_BITS);
state_t runRightToLeftState(inputs_t* inputs, uint16_t* LED_BITS);
state_t runWinningState(inputs_t* inputs);
state_t runLosingState(inputs_t* inputs);

inputs_t inputs;

int main(void){
    init();

    volatile state_t currentState = waitingToStart;
    volatile state_t nextState = currentState;


    inputs.buttonPressed = false;
    inputs.buttonTester = 0;
    uint16_t LED_BITS[7] = {BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6};

    while(1){

        switch(currentState){
        case waitingToStart:
            nextState = runWaitingState(&inputs);
            break;
        case leftToRight:
            nextState = runLeftToRightState(&inputs, LED_BITS);
            break;
        case rightToLeft:
            nextState = runRightToLeftState(&inputs, LED_BITS);
            break;
        case winning:
            nextState = runWinningState(&inputs);
            break;
        case losing:
            nextState = runLosingState(&inputs);
            break;
        default:
            break;
        }
        currentState = nextState;
    }
}

void init(){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    P2DIR |= BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5; // sets the LEDs to be outputs
    P1DIR |= BIT6;

    BUTTON_PDIR &= ~BUTTON_BIT; // sets the button as an input
    BUTTON_PREN |= BUTTON_BIT; // enables resistor for the button
    BUTTON_POUT |= BUTTON_BIT; // sets the resistor as a pull-up resistor

    P1IES |= BIT3;  // high to low trigger
    P1IE |= BIT3;   // enable interrupt
    __enable_interrupt();   // global enable
}

static inline void setAllLEDs(bool enable){
    if(enable){
        P2OUT |= BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5;
        P1OUT |= BIT6;
    } else {
        P2OUT &= ~BIT0 & ~BIT1 & ~BIT2 & ~BIT3 & ~BIT4 & ~BIT5;
        P1OUT &= ~BIT6;
    }
}

static inline void setBlueLED(bool enable){
    if(enable){
        P1OUT |= BIT6;
    } else {
        P1OUT &= ~BIT6;
    }
}

state_t runWaitingState(inputs_t* inputs){
    P2OUT = 0x00;
    //P1OUT = 0x00;

    state_t nextState = waitingToStart;
    if(inputs->buttonPressed == true){
        inputs->buttonPressed = false;
        nextState = leftToRight;
    }
    return nextState;
}

state_t runLeftToRightState(inputs_t* inputs, uint16_t* LED_BITS) {
    state_t nextState = rightToLeft;

    //runRightSweep(LED_BITS, true);

    if(inputs->buttonPressed){
        inputs->buttonTester = (inputs->buttonTester) + 10;
        inputs->buttonPressed = false;
        if (P2OUT & BIT3) {
            // go to winning state
            nextState = winning;
            return nextState;
        } else {
            // go to losing state
            nextState = losing;
            return nextState;
        }
    }

    return nextState;
}

state_t runRightToLeftState(inputs_t* inputs, uint16_t* LED_BITS){
    state_t nextState = leftToRight;

    //runLeftSweep(LED_BITS, true);

    //next state logic
    if(inputs->buttonPressed){
        inputs->buttonPressed = false;
        if(P2OUT & BIT3){
            nextState = winning;
            return nextState;
        } else {
            nextState = losing;
            return nextState;
        }
    }

    return nextState;
}

state_t runWinningState(inputs_t* inputs){
    state_t nextState = winning;

    setAllLEDs(true);
    __delay_cycles(250000);
    setAllLEDs(false);
    __delay_cycles(250000);

    if(inputs->buttonPressed){
        inputs->buttonPressed = false;
        nextState = waitingToStart;
    }
    return nextState;
}

state_t runLosingState(inputs_t* inputs){
    state_t nextState = losing;

    if(inputs->buttonPressed){
        inputs->buttonPressed = false;
        nextState = waitingToStart;
    }
    return nextState;
}

#pragma vector=PORT1_VECTOR
__interrupt void Port1ISR(){
    if(P1IFG & BIT3){
        __delay_cycles(5000); // Debounce delay
        inputs.buttonPressed = true;
        inputs.buttonTester++; // Increment tester for debug
        P1IFG &= ~BIT3; // Clear interrupt flag
    }
}


static inline void runRightSweep(uint16_t* LED_BITS, bool enable){
    if(enable){
        P2OUT |= LED_BITS[0];
        delay_ms(250);
        P2OUT &= ~(LED_BITS[0]);

        P2OUT |= LED_BITS[1];
        delay_ms(250);
        P2OUT &= ~(LED_BITS[1]);

        P2OUT |= LED_BITS[2];
        delay_ms(250);
        P2OUT &= ~(LED_BITS[2]);

        P2OUT |= LED_BITS[3];
        delay_ms(250);
        P2OUT &= ~(LED_BITS[3]);

        P2OUT |= LED_BITS[4];
        delay_ms(250);
        P2OUT &= ~(LED_BITS[4]);

        P2OUT |= LED_BITS[5];
        delay_ms(250);
        P2OUT &= ~(LED_BITS[5]);

        P1OUT |= LED_BITS[6];
        delay_ms(250);
        P1OUT &= ~(LED_BITS[6]);
    }
}

static inline void runLeftSweep(uint16_t* LED_BITS, bool enable){
    if(enable){
        volatile uint16_t i;
        for(i = (NUM_LEDS-2); i >= 1; i--){
            P2OUT |= (LED_BITS[i]);
            delay_ms(250);
            P2OUT &= ~(LED_BITS[i]);

        }
    }
}

void delay_ms(uint16_t milliseconds){
    volatile uint16_t i;
    for(i = 0; i < milliseconds; i++){
        __delay_cycles(1000);
    }
}
