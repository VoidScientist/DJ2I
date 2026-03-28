#ifndef SIMULATED
    #include <wiringPi/wiringPi.h>
#endif

#include <drivers/buttons.h>
#include <drivers/segment.h>
#include <drivers/led_matrix.h> 

#include <audio/sdl_player.h>

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

int main(int argc, char *argv[]) {

    int i           = 0;
    int number      = 0;
    char *audioName = NULL;

    if (argc > 1) {
        audioName = argv[1];
    }

    DMATRIX_setupMatrix();
    DBUTTON_setupButtons();
    DSEGMENT_setupSegment();

    sdl_player_init();

    while (1) {

        DBUTTON_scanButtons();

        if (DBUTTON_isJustPressed(0, 0)) {

            i = (i + 1) % 2;

        }
        
        if (DBUTTON_isJustPressed(0, 1)) {

            break;

        }

        if (DBUTTON_isJustPressed(0, 2)) {

            number -= 5;

        }

        if (DBUTTON_isJustPressed(0, 3)) {

            number += 5;

        }

        if (DBUTTON_isJustPressed(1, 0)) {

            sdl_player_play(audioName);

        }

        DMATRIX_clearBuffer();

        for (int j = 0; j < 8; j++) {
            DMATRIX_setRow(j, drawings[i][j]);
        }

        DSEGMENT_displayNumber(number);
        DMATRIX_renderBuffer();

#ifndef SIMULATED
        delay(10); 
#endif

    }

    DSEGMENT_setPowerState(DRIVERS_OFF);
    DMATRIX_clearMatrix();

    return 0;
}