#include <msp430.h>
#include <stdint.h>

void initClocks(){
    BCSCTL3 = LFXT1S_2;                      // LFXT1 = VLO
    IFG1 &= ~OFIFG;                           // Clear OSCFault flag
    BCSCTL2 = SELM_3 | DIVM_0;               // MCLK = LFXT1/1
}

int main(void){
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer

  initClocks();

  P1DIR |= 0x01;                            // Set P1.0 to output direction

  while(1){

    P1OUT ^= 0x01;                          // Toggle P1.0 using exclusive-OR

    __delay_cycles(32768);
  }
}
