/**
 * @file timing.h
 * @author ARCELON Louis
 * @brief fichier d'en-têtes du système de timing audio pour enregistrement.
 * @version 0.1
 * @date 2026-03-29
 * 
*/

#ifndef AUDIO_TIMING_H
#define AUDIO_TIMING_H


typedef void (*timingCallback_t)(int currentTick);



void TIMING_init(int maxTimeS, int ticksAmount, timingCallback_t callback);

void TIMING_update(int deltaMS);

void TIMING_setPause(int state);

int TIMING_getCurrentTick();

int TIMING_getMaxTicks();

void TIMING_reset();


#endif