#include <msp430.h>

int main(void){
    WDTCTL = WDT_ARST_1000;       // Set Watchdog Timer timeout 1s
    P1DIR |= 0x01;              // Set P1.0 to output direction
    P1OUT |= 0x01;              // Turn green LED on

    while(1){
        WDTCTL = WDT_ARST_1000;   // Feed the dog

        // do other stuff here
    }
}
