#include <msp430.h>
#include <stdbool.h>

#define BUZZER BIT0
#define BUTTON BIT3
#define TLED BIT6

void GPIO(void) {
    WDTCTL = WDTPW | WDTHOLD;
    P1DIR |= BUZZER|TLED;
    P1DIR &= ~BUTTON;
    P1REN |= BUTTON;
    P1OUT |= BUTTON;

    P1SEL |= BUZZER;
    P1SEL2 &= ~BUZZER;

    TA0CCR0 = 1000 - 1;
    TA0CCTL1 = OUTMOD_7;
    TA0CCR1 = 500;

    TA0CTL = TASSEL_2 | MC_0;
}

bool ButtonPressed(void) {
    return (P1IN & BUTTON) == 0;
}

int main(void) {
    GPIO();

    while (1) {
        if (ButtonPressed()) {
            P1OUT |= TLED;
            TA0CTL = TASSEL_2 | MC_1;
        } else {
            TA0CTL = TASSEL_2 | MC_0;
            P1OUT &= ~TLED;
        }

        __delay_cycles(100000);
    }
}
