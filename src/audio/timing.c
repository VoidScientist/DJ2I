/**
 * @file timing.c
 * @author ARCELON Louis
 * @brief Fichier d'implémentation pour l'API relative au timing audio (enregistrement et métronome).
 * @version 0.1
 * @date 2026-03-29
 * 
*/

#include <stdio.h>
#include <SDL2/SDL.h>
#include <audio/timing.h>

static int maxRecordTimeMS;
static int maxTick;

static int tickTimeMS;

static timingCallback_t callback;

static int currentTick = 0;

static int wasSetup = 0;


void TIMING_init(int maxTimeS, int ticksAmount, timingCallback_t callbackNew) {

    maxRecordTimeMS = maxTimeS * 1000;
    maxTick = ticksAmount;

    tickTimeMS = maxRecordTimeMS / maxTick;

    callback = callbackNew;

    if (callback == NULL) {
        printf("[timing] Callback ne doit pas être NULL\n");
        return;
    }

    printf("[timing] Timing initialisé avec succès.\n");
    wasSetup = 1;

}

void TIMING_update(int deltaMS) {
    static int timer = 0;

    if (!wasSetup) {

        printf("[timing] Utilisez TIMING_init() avant d'utiliser les fonctions de TIMING.\n");
    
        return;
        
    }

    timer += deltaMS;

    while (timer >= tickTimeMS) {
        callback(currentTick);
        currentTick = (currentTick + 1) % maxTick;
        timer -= tickTimeMS;
    }

}

int TIMING_getCurrentTick() {
    if (!wasSetup) {

        printf("[timing] Utilisez TIMING_init() avant d'utiliser les fonctions de TIMING.\n");
    
        return -1;
        
    }
    return currentTick;
}


int TIMING_getMaxTicks() {
    if (!wasSetup) {

        printf("[timing] Utilisez TIMING_init() avant d'utiliser les fonctions de TIMING.\n");
    
        return -1;
        
    }
    return maxTick;
}

