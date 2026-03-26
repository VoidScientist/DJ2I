#ifndef SIMULATED
    #include <wiringPi/wiringPi.h>
#endif

#include <drivers/buttons.h>
#include <drivers/led_matrix.h> 

int drawings[2][8] = {
    {
        0b00111100,
        0b01000010,
        0b10100101,
        0b10000001,
        0b10100101,
        0b10011001,
        0b01000010,
        0b00111100
    },
    {
        0b00111100,
        0b01000010,
        0b10100101,
        0b10000001,
        0b10011001,
        0b10100101,
        0b01000010,
        0b00111100
    }

};

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

            i = (i + 1) % 2;

        }
        
        if (DBUTTON_isJustPressed(0, 1)) {

            break;

        }

        DMATRIX_clearBuffer();

        for (int j = 0; j < 8; j++) {
            DMATRIX_setRow(j, drawings[i][j]);
        }

        DMATRIX_renderBuffer();

#ifndef SIMULATED
        delay(10); 
#endif

    }

    DMATRIX_clearMatrix();

    return 0;
}