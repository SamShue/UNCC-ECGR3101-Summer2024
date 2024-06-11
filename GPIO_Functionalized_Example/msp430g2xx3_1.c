#include <msp430.h>
#include "hal.h"

int main(){
    init();

    while(1){
        if(isButtonPressed_Debounced()){
            setGreenLed(true);
            setRedLed(true);
        } else {
            setGreenLed(false);
            setRedLed(false);
        }
    }
}
