/**
 * @file segment.c
 * @author ARCELON Louis
 * @brief Fichier implémentation de l'API du sept segments.
 * @version 0.1
 * @date 2026-03-27
 * 
*/

#ifndef SIMULATED
    #include <stdio.h>
    #include <wiringPi/wiringPi.h>
    #include <wiringPi/wiringPiI2C.h>
    #include <drivers/drivers_common.h>
    #include <drivers/segment.h> 
#else
    #include <stdio.h>
    #include <drivers/drivers_common.h>
    #include <drivers/segment.h> 
#endif

#define DIGIT_NUMB 4

#define SEG_ADDR 0x70
#define SEG_DIG1 0x08
#define SEG_DIG2 0x06
#define SEG_COLON 0x04
#define SEG_DIG3 0x02
#define SEG_DIG4 0x00



static int digits[10] = {

    0x3F,
    0x06,
    0x5B,
    0x4F,
    0x66,
    0x6D,
    0x7D,
    0x07,
    0x7F,
    0x6F

};

static int minusSign = 0x40;

static int seg_digits[4] = {SEG_DIG1, SEG_DIG2, SEG_DIG3, SEG_DIG4};

static int seg_fd;

static int wasSetup;


int DSEGMENT_setupSegment() {

    DCOMMON_initWPi();

#ifndef SIMULATED

    if ((seg_fd = wiringPiI2CSetup(SEG_ADDR)) < 0) {
        printf("Récupération seg_fd échouée.\n");
        return -1;
    }

    printf("[DSEGMENT] Initialisé en mode: HARDWARE.\n");
    wasSetup = 1;
    
    DSEGMENT_setPowerState(DRIVERS_ON);
    DSEGMENT_setDisplay(DRIVERS_ON, S_NOBLINK);

    // luminosité par défaut, pas changeable dans l'API.
    wiringPiI2CWrite(seg_fd, 0xE5); 

#else 

    printf("[DSEGMENT] Initialisé en mode: SIMULATION.\n");
    wasSetup = 1;

#endif

}



void DSEGMENT_setPowerState(powerState_t state) {

    if (!wasSetup) {
        printf("[ERREUR] DSEGMENT_setPowerState(): Drivers non initialisé.\n");
        return;
    }

    int code = 0x20 | state;

#ifndef SIMULATED
    wiringPiI2CWrite(seg_fd, code);
#else
    printf("[DSEGMENT] On envoie le code: %02x.\n", code);
#endif

}



void DSEGMENT_setDisplay(powerState_t state, blinkMode_t blink) {

    if (!wasSetup) {
        printf("[ERREUR] DSEGMENT_setDisplay(): Drivers non initialisé.\n");
        return;
    }

    int code = 0x80 | (state > 0) | (blink << 1);

#ifndef SIMULATED
    wiringPiI2CWrite(seg_fd, code);
#else
    printf("[DSEGMENT] On envoie le code: %02x.\n", code);
#endif

}



void DSEGMENT_displayNumber(int number) {

    int availableDigits = DIGIT_NUMB;

    if (!wasSetup) {
        printf("[ERREUR] DSEGMENT_displayNumber(): Drivers non initialisé.\n");
        return;
    }

    if (number < 0) {
        number = -number;
        availableDigits--;
        wiringPiI2CWriteReg8(seg_fd, seg_digits[DIGIT_NUMB-1], minusSign);
    }

    for (int i = 0; i < availableDigits; i++) {

#ifndef SIMULATED
        wiringPiI2CWriteReg8(seg_fd, seg_digits[i], digits[number % 10]);
#else
        printf("[DSEGMENT] On envoie au registre %02x: %02x\n", seg_digits[i], digits[number % 10]);
#endif

        number /= 10;

    }

}