#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "audio/sdl_player.h"
#include "audio/fft_engine.h"
#include "audio/spectrum_mapper.h"

#include "wiringPi/wiringPi.h"

#include "drivers/led_matrix.h"

#define TARGET_FPS 25
#define FRAME_MS   (1000 / TARGET_FPS)
#define DEFAULT_WAV "test.wav"

int main(int argc, char *argv[])
{
    const char *wav_path;
    float pcm[PLAYER_FRAME_SIZE];
    float magnitudes[FFT_BINS];
    BandFrame_t band_frame;
    Uint32 t0, elapsed;

    wav_path = (argc > 1) ? argv[1] : DEFAULT_WAV;

    DMATRIX_setupMatrix();
    DMATRIX_setBrightness(M_VERY_DIM);

    if (sdl_player_init() != 0)
        return EXIT_FAILURE;

    if (fft_engine_init() != 0) {
        sdl_player_cleanup();
        return EXIT_FAILURE;
    }

    spectrum_mapper_init();

    sdl_player_play(wav_path);

    printf("Lecture : %s\n", wav_path);

    while (sdl_player_is_playing()) {

        t0 = SDL_GetTicks();

        if (sdl_player_read_frame(pcm, PLAYER_FRAME_SIZE)) {

            fft_engine_compute(pcm, magnitudes);
            spectrum_mapper_compute(magnitudes, &band_frame);


            for (int b = 0; b < NUM_BANDS; b++) {

                unsigned char command = (1 << band_frame.heights[b]) - 1;

                DMATRIX_setColumn(b * 2, command);
                DMATRIX_setColumn(b * 2 + 1, command);

            }

            DMATRIX_renderBuffer();

        }

        elapsed = SDL_GetTicks() - t0;
        if (elapsed < FRAME_MS)
            SDL_Delay(FRAME_MS - elapsed);
    }

    DMATRIX_clearMatrix();

    fft_engine_cleanup();
    sdl_player_cleanup();

    printf("\nFin de lecture.\n");

    return EXIT_SUCCESS;
}
