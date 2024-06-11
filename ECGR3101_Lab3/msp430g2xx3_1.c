#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>

#define MCLK_FREQ_HZ    1000000
#define LED_DELAY_MS    100

bool buttonFlag = false;

typedef enum {RUNNING_UP, RUNNING_DOWN, SCORE, MAX_STATES} state_t;

typedef struct{
    bool isButtonPressed;
    uint8_t ledPosition;
    uint16_t milliSeconds;
} inputs_t;

state_t runRunningUpState(inputs_t* inputs);
state_t runRunningDownState(inputs_t* inputs);
state_t runScoreState(inputs_t* inputs);

void init();
bool readButtonPress();
void setLED(uint8_t pos);
void delay_ms(uint16_t millis);

int main(void){
    state_t currentState = RUNNING_UP;
    state_t nextState = currentState;

    inputs_t inputs;
    inputs.isButtonPressed = false;
    inputs.ledPosition = 0;
    inputs.milliSeconds = 0;

    init();

    while(1){
        switch(currentState){
        case RUNNING_UP:
            nextState = runRunningUpState(&inputs);
            break;
        case RUNNING_DOWN:
            nextState = runRunningDownState(&inputs);
            break;
        case SCORE:
            nextState = runScoreState(&inputs);
            break;
        }

        inputs.isButtonPressed = buttonFlag;
        buttonFlag = false;
        currentState = nextState;
    }
}

state_t runRunningUpState(inputs_t* inputs){
    setLED(inputs->ledPosition);
    delay_ms(1);
    inputs->milliSeconds++;

    if(inputs->milliSeconds > LED_DELAY_MS){
        inputs->ledPosition++;
        inputs->milliSeconds = 0;
    }

    if(inputs->isButtonPressed){
        inputs->isButtonPressed = false;
        return SCORE;
    }

    if(inputs->ledPosition >= 6){
        return RUNNING_DOWN;
    }

    return RUNNING_UP;
}

state_t runRunningDownState(inputs_t* inputs){
    setLED(inputs->ledPosition);
    delay_ms(1);
    inputs->milliSeconds++;

    if(inputs->milliSeconds > LED_DELAY_MS){
        inputs->ledPosition--;
        inputs->milliSeconds = 0;
    }

    if(inputs->isButtonPressed){
        inputs->isButtonPressed = false;
        return SCORE;
    }

    if(inputs->ledPosition == 0){
        return RUNNING_UP;
    }

    return RUNNING_DOWN;
}

state_t runScoreState(inputs_t* inputs){
    inputs->milliSeconds = 0;

    if(inputs->isButtonPressed){
        inputs->isButtonPressed = false;
        return RUNNING_UP;
    } else {
        return SCORE;
    }
}

void delay_ms(uint16_t millis){
    volatile uint16_t i;
    for(i = 0; i < millis; i++){
        __delay_cycles(MCLK_FREQ_HZ/1000);
    }
}

void setLED(uint8_t pos){
    switch(pos){
    case 6:
        P2OUT &= (BIT6 | BIT7);
        P1OUT |= BIT6;
        break;
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        P1OUT &= ~BIT6;
        P2OUT = 1 << pos;
        break;
    }
}

bool readButtonPress(){
    if((P1IN & BIT3) == 0x00){
        delay_ms(100);
        return (P1IN & BIT3) == 0x00;
    }

    return false;
}

void init(){
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    // Init LEDs
    P2DIR = (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5);
    P1DIR = BIT6;

    // Init Button
    P1DIR &= ~BIT3;
    P1REN |= BIT3;
    P1OUT |= BIT3;
    P1IE |= BIT3;
    P1IES |= BIT3;
    P1IFG &= ~BIT3;

    __enable_interrupt();
}

#pragma vector=PORT1_VECTOR
__interrupt void port1ISR(){
    delay_ms(100);
    buttonFlag = (P1IN & BIT3) == 0x00;

    P1IFG &= ~BIT3;
}
