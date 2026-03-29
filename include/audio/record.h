/**
 * @file record.h
 * @author ARCELON Louis
 * @brief Fichier d'en-tête du système de recording d'appuis du looper.
 * @version 0.1
 * @date 2026-03-30
 * 
 * @copyright Copyright (c) 2026
 * 
*/

#ifndef AUDIO_RECORD_H
#define AUDIO_RECORD_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

typedef struct {
    Mix_Chunk *chunk;
    int tick;
} recordData_t;


void RECORD_recordPress(Mix_Chunk *data, int currentTick);

void RECORD_playRecorded(int currentTick);


#endif