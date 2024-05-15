#include <msp430.h>

int main(){
    WDTCTL = 0x5A80; // Stop the watchdog timer

    P1DIR = ~BIT3;
    P1REN = BIT3;
    P1OUT = BIT3;

    while(1){
        if((P1IN & BIT3) == 0x00){ // Masks P1 with 0b0000 1000 to clear all bits other than bit3
            P1OUT |= BIT0;   // Masks P1OUT with 0b0000 0001 to set bit0
        } else {
            P1OUT &= ~BIT0;   // Masks P1OUT with 0b1111 1110 to clear bit0
        }
    }
}
