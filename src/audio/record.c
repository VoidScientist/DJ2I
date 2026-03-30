/**
 * @file record.c
 * @author ARCELON Louis
 * @brief fichier d'implémentation du système d'enregistrement d'appuis pour looper.
 * @version 0.1
 * @date 2026-03-30
 * 
*/

#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "audio/sdl_player.h"
#include <audio/record.h>


recordData_t recordedPresses[MAX_RECORDINGS];

static int recorded = 0;
static int cursor = 0;
static int currentChannel = 0;


static int canChannelPlay(int channel) {

    return channel <= currentChannel;

}


void RECORD_recordPress(recordNamedChunk_t *data, int currentTick) {

    recordData_t chunkData;

    if (data == NULL) {
        printf("[record] recordNamedChunk_t à enregistrer ne peut pas être nul.\n");
        return;
    }

    if (cursor >= MAX_RECORDINGS) {
        cursor = 0;
    }
    
    strncpy(chunkData.name, data->name ? data->name : "N/a", MAX_RECORD_NAME_LENGTH);
    chunkData.chunk = data->chunk;
    chunkData.tick = currentTick;
    chunkData.channel = currentChannel;


    recordedPresses[cursor++] = chunkData;
    
    if (recorded < MAX_RECORDINGS) {
        recorded++;
    }

}


void RECORD_playRecorded(int currentTick) {
    recordData_t data;

    for (int i = 0; i < recorded; i++) {

        data = recordedPresses[i];

        if (data.tick == currentTick && canChannelPlay(data.channel)) {
            sdl_player_play_chunk(data.chunk);
        }
        
    }

}


void RECORD_clearRecordings() {

    recorded = cursor = 0;

}


void RECORD_setCurrentChannel(int channel) {
    if (channel < 0 || channel >= MAX_RECORD_CHANNELS) return;

    currentChannel = channel;

}


void RECORD_getActiveChannelsArray(int *channels) {

    for (int i = 0; i < MAX_RECORD_CHANNELS; i++) {
        channels[i] = canChannelPlay(i);
    }

}


void RECORD_getRecordedPresses(uiRecordedPress **arr, int *pressCounts) {

    uiRecordedPress tmp;
    recordData_t data;

    for (int i = 0; i < MAX_RECORD_CHANNELS; i++) {
        pressCounts[i] = 0;
    }

    for (int i = 0; i < recorded; i++) {

        data = recordedPresses[i];

        strncpy(tmp.name, data.name, MAX_RECORD_NAME_LENGTH);
        tmp.time = data.tick;

        arr[data.channel][pressCounts[data.channel]++] = tmp; 

    }


}