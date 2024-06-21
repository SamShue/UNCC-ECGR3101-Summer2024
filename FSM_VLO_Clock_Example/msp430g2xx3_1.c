#include <msp430.h>
#include <stdint.h>

void initClocks(){

    BCSCTL3 = LFXT1S_2;                      // LFXT1 = VLO
    IFG1 &= ~OFIFG;                           // Clear OSCFault flag
    BCSCTL2 = SELM_3 | DIVM_1;               // MCLK = LFXT1/2

    /*
    // Set bit5 = 1 and bit4 = 0 to select VLO clock as LFXT1 source
    BCSCTL3 |= BIT5;b
    BCSCTL3 &= ~BIT4;

    // Clear the fault flag
    IFG1 &= ~BIT1;

    // Select MCLK signal as LFXT1 source
    BCSCTL2 |= (BIT7 | BIT6);
    */
}

int main(void){
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer

  initClocks();

  P1DIR |= 0x01;                            // Set P1.0 to output direction

  while(1){

    P1OUT ^= 0x01;                          // Toggle P1.0 using exclusive-OR

    __delay_cycles(6000);
  }
}
