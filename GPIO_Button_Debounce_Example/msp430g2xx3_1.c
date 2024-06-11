#include <msp430.h>
#include <stdbool.h>

bool isButtonPressed_NoDebounce(){
    return (P1IN & BIT3) == 0x00;
}

bool isButtonPressed_DelayDebounce(){
    volatile int i;
    if((P1IN & BIT3) == 0x00){
        for(i = 0; i < 10000; i++);
        if((P1IN & BIT3) == 0x00){
            return true;
        }
    }
    return false;
}

bool isButtonPressed_StateDebounce() {
  static unsigned int state = 0;
  state = (state<<1) | ((P1IN & BIT3) == 0x00) | 0xfe00;
  return (state == 0xff00);
}

int main(){
    WDTCTL = 0x5A80; // Stop the watchdog timer

    P1DIR &= ~BIT3;
    P1REN |= BIT3;
    P1OUT |= BIT3;

    P2DIR |= BIT1 | BIT3 | BIT5;
    P2OUT &= ~(BIT1 | BIT3 | BIT5);

    volatile int count = 0;
    while(1){
        if(isButtonPressed_DelayDebounce()){
            count = count >= 2 ? 0 : count + 1;
        }

        switch(count){
        case 0:
            P2OUT |= BIT1;
            P2OUT &= ~(BIT3 | BIT5);
            break;
        case 1:
            P2OUT |= BIT3;
            P2OUT &= ~(BIT1 | BIT5);
            break;
        case 2:
            P2OUT |= BIT5;
            P2OUT &= ~(BIT1 | BIT3);
            break;
        }
    }
}
