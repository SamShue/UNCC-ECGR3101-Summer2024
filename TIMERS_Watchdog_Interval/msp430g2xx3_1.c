#include <msp430.h>

int main(void){
    WDTCTL = WDT_MDLY_32;                     // Set Watchdog Timer interval to ~30ms
    IE1 |= WDTIE;                             // Enable WDT interrupt
    P1DIR |= 0x01;                            // Set P1.0 to output direction

    __enable_interrupt();

    while(1){

    }
}

#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void) {
    P1OUT ^= 0x01;                            // Toggle P1.0 using exclusive-OR
}
