#include <msp430.h>

int main(void){
  WDTCTL = WDTPW + WDTHOLD;       // Stop watchdog timer

  P1DIR |= BIT6;    // Red LED Output
  P1SEL |= BIT6;    // Red LED PWM

  TA0CCR0 = 512;    // PWM Period
  TA0CCR1 = 100;    // PWM duty cycle
  TA0CCTL1 = OUTMOD_7;  // TA0CCR1 high voltage below count, low voltage above count

  TA0CTL = TASSEL_2 | MC_1 | ID_3;

  while(1){
  }
}
