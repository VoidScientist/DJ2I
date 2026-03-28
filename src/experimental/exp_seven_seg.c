/**
 * @file exp_seven_seg.c
 * @author ARCELON Louis
 * @brief Fichier expérimental avant la création de l'API driver du sept segment pour le BPM
 * @version 0.1
 * @date 2026-03-27
 * 
*/

#include <stdlib.h>
#include "wiringPi/wiringPi.h"

#include <drivers/segment.h>

int main(int argc, char *argv[]) {

    int number = 0;

    if (argc > 1) {
        number = atoi(argv[1]);
    }

    if (DSEGMENT_setupSegment() < 0) {
        return -1;
    }

    DSEGMENT_displayNumber(number);

#ifndef SIMULATED
    delay(2000);
#endif
    
    DSEGMENT_setDisplay(0, 0);

    return 0;

}



