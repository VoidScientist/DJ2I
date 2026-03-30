/**
 * @file buzzer.c
 * @author ARCELON Louis
 * @brief Fichier d'implémentation du buzzer.
 * @version 0.1
 * @date 2026-03-29
 *
*/

#include <stdio.h>

#ifndef SIMULATED
    #include <wiringPi/wiringPi.h>
    #include <wiringPi/softTone.h>
#endif

#include <drivers/drivers_common.h>
#include <drivers/buzzer.h>


#define BUZZER_GPIO 12

static int wasSetup = 0;


void DBUZZER_setupBuzzer() {

    DCOMMON_initWPi();

#ifndef SIMULATED
    pinMode(BUZZER_GPIO, OUTPUT);
    softToneCreate(BUZZER_GPIO);
#endif

    wasSetup = 1;

}


void DBUZZER_setBeep(int state) {

    if (!wasSetup) {
        printf("[DBUZZER] Veuillez initialiser le buzzer avant de l'utiliser.\n");
        return;
    }

#ifndef SIMULATED
    softToneWrite(BUZZER_GPIO, state ? 1000 : 0);
#else

    char *msg = state ? "fait bip" : "ne fait plus bip";

    printf("[DBUZZER] Le buzzer %s.\n", msg);

#endif

}