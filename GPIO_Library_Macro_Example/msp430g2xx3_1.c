#include <msp430.h>

int main(){
    WDTCTL = 0x5A80; // Stop the watchdog timer

    P1DIR = 0xF7; // writes 11110111 to p1 direction register
    P1REN = 0x08; // writes 00001000 to p1 resistor enable register
    P1OUT = 0x08; // writes 00001000 to p1 output register

    while(1){
        if(P1IN == 0x00){
            P1OUT = 0x09; // 00001001 (LED on, pull-up enabled)
        } else {
            P1OUT = 0x08; // 00001000 (LED off, pull-up enabled)
        }
    }
}
