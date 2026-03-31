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

#include <ui/ui_types.h>

#define MAX_RECORDINGS 256

#define MAX_RECORD_CHANNELS 4
#define MAX_RECORD_NAME_LENGTH 16


typedef struct {

    char name[MAX_RECORD_NAME_LENGTH];
    Mix_Chunk *chunk;

} recordNamedChunk_t;


typedef struct {
    char name[MAX_RECORD_NAME_LENGTH];
    Mix_Chunk *chunk;
    int tick;
    int channel;
} recordData_t;


void RECORD_recordPress(recordNamedChunk_t *data, int currentTick);

void RECORD_playRecorded(int currentTick);

void RECORD_clearRecordings();

void RECORD_setCurrentChannel(int channel);

void RECORD_getActiveChannelsArray(int *channels);

void RECORD_getRecordedPresses(uiRecordedPress **arr, int *pressCounts);

#endif