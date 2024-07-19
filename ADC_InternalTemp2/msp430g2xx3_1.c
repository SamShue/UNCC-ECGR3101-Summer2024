#include <msp430.h>

long temp;
long IntDegF;
long IntDegC;

int main(void) {
    WDTCTL = WDTPW + WDTHOLD;

    ADC10CTL1 = INCH_10 | ADC10DIV_3;         // Temp Sensor ADC10CLK/4
    ADC10CTL0 = SREF_1 | ADC10SHT_3 | REFON | ADC10ON;
    __enable_interrupt();

    while(1){
        ADC10CTL0 |= ENC | ADC10SC;             // Sampling and conversion start
        while(ADC10CTL1 & ADC10BUSY);

        // oF = ((A10/1024)*1500mV)-923mV)*1/1.97mV = A10*761/1024 - 468
        temp = ADC10MEM;
        IntDegF = ((temp - 630) * 761) / 1024;

        // oC = ((A10/1024)*1500mV)-986mV)*1/3.55mV = A10*423/1024 - 278
        temp = ADC10MEM;
        IntDegC = ((temp - 673) * 423) / 1024;

        __no_operation();                       // SET BREAKPOINT HERE

    }
}
