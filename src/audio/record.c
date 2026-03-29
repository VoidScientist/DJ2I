/**
 * @file record.c
 * @author ARCELON Louis
 * @brief fichier d'implémentation du système d'enregistrement d'appuis pour looper.
 * @version 0.1
 * @date 2026-03-30
 * 
*/

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "audio/sdl_player.h"
#include <audio/record.h>

#define MAX_RECORDINGS 256

recordData_t recordedPresses[MAX_RECORDINGS];

static int recorded = 0;
static int cursor = 0;


void RECORD_recordPress(Mix_Chunk *data, int currentTick) {

    recordData_t chunkData;

    if (data == NULL) {
        printf("[record] chunk à enregistrer ne peut pas être nul.\n");
        return;
    }

    if (cursor >= MAX_RECORDINGS) {
        cursor = 0;
    }
    
    chunkData.chunk = data;
    chunkData.tick = currentTick;


    recordedPresses[cursor++] = chunkData;
    
    if (recorded < MAX_RECORDINGS) {
        recorded++;
    }

}


void RECORD_playRecorded(int currentTick) {

    for (int i = 0; i < recorded; i++) {

        if (recordedPresses[i].tick == currentTick) {
            sdl_player_play_chunk(recordedPresses[i].chunk);
        }
        
    }

}