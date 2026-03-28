/**
 * @file drivers_common.c
 * @author ARCELON Louis
 * @brief Fichier d'implémentation des fonctions communes aux drivers.
 * @version 0.1
 * @date 2026-03-27
 * 
*/

#ifndef SIMULATED
    #include <stdio.h>
    #include <wiringPi/wiringPi.h>
#else
    #include <stdio.h>
#endif

static int wasSetup = 0;

void DCOMMON_initWPi() {

    if (wasSetup) return;

#ifndef SIMULATED
    wiringPiSetupGpio();
    printf("[DCOMMON] wiringPi initialisé en mode GPIO.\n");
#else
    printf("[DCOMMON] wiringPi non initialisé, cause: SIMULATION.\n");
#endif

    wasSetup = 1;

}
