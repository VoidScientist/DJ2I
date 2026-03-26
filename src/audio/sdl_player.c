/**
 *  \file       sdl_player.c
 *  \brief      Implémentation de la capture audio avec SDL2
 *  \author     MARTEL Mathieu
 *  \version    2.0
 */

/*
*****************************************************************************************
 *  \noop       I N C L U D E S
 */
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

#include "audio/sdl_player.h"

/*
*****************************************************************************************
 *  \noop       C O N S T A N T E S
 */

/* Taille du ring buffer : on garde 16 trames d'avance */
#define RING_SIZE   (PLAYER_FRAME_SIZE * 16)

/*
*****************************************************************************************
 *  \noop       V A R I A B L E S   G L O B A L E S
 */

/** @brief Buffer circulaire pour stocker les échantillons mono */
static int16_t      s_ring[RING_SIZE];

/** @brief Position d'écriture dans le ring buffer */
static size_t       s_write_pos = 0;

/** @brief Nombre total d'échantillons écrits depuis le début */
static uint64_t     s_total_written = 0;

/** @brief Mutex pour éviter les accès concurrents au buffer */
static SDL_mutex    *s_mutex = NULL;

/*
*****************************************************************************************
 *  \noop       F O N C T I O N S   I N T E R N E S
 */

/**
 * @brief Callback appelé par SDL après chaque mix audio
 *
 * Capture tout ce que SDL_mixer joue (sons de boutons, fichiers WAV...)
 * et remplit le ring buffer après downmix stéréo -> mono.
 *
 * @param udata     Non utilisé
 * @param stream    Données PCM brutes (format S16 stéréo)
 * @param len       Taille du buffer en octets
 */
static void post_mix_callback(void *udata, Uint8 *stream, int len)
{
    int16_t *samples = (int16_t *)stream;
    int nb_frames    = len / (2 * sizeof(int16_t));
    int32_t mono_sample;

    (void)udata;

    SDL_LockMutex(s_mutex);

    for (int i = 0; i < nb_frames; i++) {
        /* Downmix stéréo -> mono par moyenne des deux canaux */
        mono_sample         = ((int32_t)samples[i * 2] + (int32_t)samples[i * 2 + 1]) / 2;
        s_ring[s_write_pos] = (int16_t)mono_sample;
        s_write_pos         = (s_write_pos + 1) % RING_SIZE;
        s_total_written++;
    }

    SDL_UnlockMutex(s_mutex);
}

/*
*****************************************************************************************
 *  \noop       F O N C T I O N S   P U B L I Q U E S
 */

int sdl_player_init(void)
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "[sdl_player] Erreur SDL_Init : %s\n", SDL_GetError());
        return -1;
    }

    if (Mix_OpenAudio(PLAYER_SAMPLE_RATE, AUDIO_S16SYS, PLAYER_CHANNELS, PLAYER_CHUNK_SIZE) < 0) {
        fprintf(stderr, "[sdl_player] Erreur Mix_OpenAudio : %s\n", Mix_GetError());
        SDL_Quit();
        return -1;
    }

    s_mutex = SDL_CreateMutex();
    if (s_mutex == NULL) {
        fprintf(stderr, "[sdl_player] Erreur SDL_CreateMutex : %s\n", SDL_GetError());
        Mix_CloseAudio();
        SDL_Quit();
        return -1;
    }

    /* On branche notre callback pour capturer tout ce que SDL_mixer joue */
    Mix_SetPostMix(post_mix_callback, NULL);

    printf("[sdl_player] Init OK (%d Hz, %d canaux)\n",
        PLAYER_SAMPLE_RATE, PLAYER_CHANNELS);
    return 0;
}

int sdl_player_play(const char *wav_path)
{
    Mix_Chunk   *chunk;
    int         channel;

    chunk = Mix_LoadWAV(wav_path);
    if (chunk == NULL) {
        fprintf(stderr, "[sdl_player] Impossible de charger '%s' : %s\n",
            wav_path, Mix_GetError());
        return -1;
    }

    channel = Mix_PlayChannel(-1, chunk, 0);
    if (channel < 0) {
        fprintf(stderr, "[sdl_player] Erreur Mix_PlayChannel : %s\n", Mix_GetError());
        Mix_FreeChunk(chunk);
        return -1;
    }

    printf("[sdl_player] Lecture de '%s' (canal %d)\n", wav_path, channel);
    return 0;
}

void sdl_player_cleanup(void)
{
    Mix_SetPostMix(NULL, NULL);
    Mix_HaltChannel(-1);

    if (s_mutex != NULL) {
        SDL_DestroyMutex(s_mutex);
        s_mutex = NULL;
    }

    Mix_CloseAudio();
    SDL_Quit();

    printf("[sdl_player] Nettoyage OK\n");
}

bool sdl_player_is_playing(void)
{
    return Mix_Playing(-1) > 0;
}

bool sdl_player_read_frame(float *out, size_t count)
{
    size_t start_pos;

    SDL_LockMutex(s_mutex);

    if (s_total_written < count) {
        SDL_UnlockMutex(s_mutex);
        return false;
    }

    /* On remonte dans le ring buffer pour récupérer les 'count' derniers échantillons */
    start_pos = (s_write_pos + RING_SIZE - count) % RING_SIZE;

    for (size_t i = 0; i < count; i++) {
        /* Normalisation int16 -> float entre -1.0 et 1.0 */
        out[i] = (float)s_ring[(start_pos + i) % RING_SIZE] / 32768.0f;
    }

    SDL_UnlockMutex(s_mutex);
    return true;
}