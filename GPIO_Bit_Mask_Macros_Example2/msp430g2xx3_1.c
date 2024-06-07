#include <msp430.h>

// Set the green LED (P1.0) and the red LED (P1.6) when pressed, and both off when not pressed
int main(){
    WDTCTL = 0x5A80; // Stop the watchdog timer

    P1DIR = (BIT0 | BIT6);
    //P1DIR &= ~BIT3;
    P1REN = BIT3;
    P1OUT = BIT3;

    while(1){
        if((P1IN & BIT3) == 0x00){ // Masks P1 with 0b0000 1000 to clear all bits other than bit3
            P1OUT |= (BIT0 | BIT6);   // Turn on green and red LED
        } else {
            P1OUT &= ~(BIT0 | BIT6);   // Turn off green and red LED
        }
    }
}
