
#include <stdlib.h>

#ifndef SIMULATED
    #include <wiringPi/wiringPi.h>
#endif

#include <drivers/buttons.h>
#include <drivers/led_matrix.h> 


int main(void) {

    int i = 0;

#ifndef SIMULATED
    wiringPiSetupGpio();
#endif


    DMATRIX_setupMatrix();
    DBUTTON_setupButtons();

    while (1) {

        DBUTTON_scanButtons();

        if (DBUTTON_isJustPressed(0, 0)) {

            i = (i + 1) % 8;

        }
        
        if (DBUTTON_isJustPressed(0, 1)) {

            i = abs(i + 7) % 8;

        }

        DMATRIX_clearBuffer();

        DMATRIX_setColumn(i, 0b11100000);

        DMATRIX_renderBuffer();

#ifndef SIMULATED
        delay(10); 
#endif

    }

    return 0;
}