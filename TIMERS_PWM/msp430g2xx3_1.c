#include <msp430.h>
#include <stdint.h>

int i = 0;
int a[] = {0, 32, 64, 128, 255};

int main(void){
  WDTCTL = WDTPW + WDTHOLD;       // Stop watchdog timer

  P1DIR |= BIT6;    // Green LED Output
  P1SEL |= BIT6;    // Green LED PWM

  TA0CCR0 = 512;    // PWM Period
  TA0CCR1 = 32;   // PWM duty cycle
  TA0CCTL1 = OUTMOD_7;  // TA0CCR1 high voltage below count, low voltage above count

  TA0CTL = TASSEL_2 | MC_1 | TAIE | ID_3;

  __enable_interrupt();

  while(1){
  }
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A (void){
    TA0CTL &= ~TAIFG;
    /*
    TA0CCR1 = a[++i];
    _delay_cycles(500000);
    if(i > 4){
        i = 0;
    }
    */
}
