#include <msp430.h>
#include <stdint.h>

void delay_s(uint16_t seconds){
    volatile uint16_t i;
    for(int i = 0; i < seconds; i++){
        __delay_cycles(1000000);
    }
}

int main(void){
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  P1DIR |= 0x01;                            // Set P1.0 to output direction

  while(1) {
    P1OUT ^= 0x01;                          // Toggle P1.0 using exclusive-OR

    delay_s(1);
  }
}
