/**
 * @file segment.h
 * @author ARCELON Louis
 * @brief Fichier en-têtes de l'API du sept segment 4 chiffres.
 * @version 0.1
 * @date 2026-03-27
 * 
 */

#ifndef DSEGMENT_H
#define DSEGMENT_H

#include <drivers/drivers_common.h>

typedef enum {S_NOBLINK, S_FASTBLINK, S_BLINK, S_SLOWBLINK} blinkMode_t;


int DSEGMENT_setupSegment();

void DSEGMENT_setPowerState(powerState_t state);

void DSEGMENT_setDisplay(powerState_t state, blinkMode_t blink);

void DSEGMENT_displayNumber(int number);


#endif /** DSEGMENT_H **/