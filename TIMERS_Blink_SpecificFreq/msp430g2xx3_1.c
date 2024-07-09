#include <msp430.h>
#include <stdint.h>

void initTimer(){
    // Configure Timer to use SMCLK in Up to CCR0 mode
    CCTL0 = CCIE;                 // CCR0 interrupt enabled
    CCR0 = 4167;                 // Count value for 1s delay @ 12kHz
    TACTL = TASSEL_2 | MC_1;      // SMCLK, upmode
}

int main(void){
  WDTCTL = WDTPW + WDTHOLD;       // Stop watchdog timer

  initTimer();

  P1DIR |= 0x01;                 // Set P1.0 to output direction

  __enable_interrupt();

  while(1){
  }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void){
    P1OUT ^= 0x01;               // Toggle P1.0 using exclusive-OR
}
