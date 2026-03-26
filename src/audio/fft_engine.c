/**
 *  \file       fft_engine.c
 *  \brief      Implémentation du calcul FFT avec KissFFT
 *  \author     MARTEL Mathieu
 *  \version    1.0
 */

/*
*****************************************************************************************
 *  \noop       I N C L U D E S
 */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "kissfft/kiss_fftr.h"
#include "audio/fft_engine.h"

/*
*****************************************************************************************
 *  \noop       C O N S T A N T E S
 */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
*****************************************************************************************
 *  \noop       V A R I A B L E S   G L O B A L E S
 */

/** @brief Configuration KissFFT */
static kiss_fftr_cfg    s_fft_cfg = NULL;

/** @brief Fenêtre de Hann pour réduire le leakage spectral */
static float            *s_window = NULL;

/** @brief Buffer de sortie FFT (valeurs complexes) */
static kiss_fft_cpx     *s_fft_out = NULL;

/*
*****************************************************************************************
 *  \noop       F O N C T I O N S   P U B L I Q U E S
 */

/**
 * @brief Initialise KissFFT et précalcule la fenêtre de Hann
 *
 * @return 0 si ok, -1 si erreur d'allocation
 */
int fft_engine_init(void)
{
    int i;

    s_fft_cfg = kiss_fftr_alloc(FFT_SIZE, 0, NULL, NULL);
    if (s_fft_cfg == NULL) {
        fprintf(stderr, "[fft_engine] Erreur kiss_fftr_alloc\n");
        return -1;
    }

    s_window = (float *)malloc(FFT_SIZE * sizeof(float));
    if (s_window == NULL) {
        fprintf(stderr, "[fft_engine] Erreur malloc fenetre\n");
        free(s_fft_cfg);
        return -1;
    }

    s_fft_out = (kiss_fft_cpx *)malloc(FFT_BINS * sizeof(kiss_fft_cpx));
    if (s_fft_out == NULL) {
        fprintf(stderr, "[fft_engine] Erreur malloc sortie FFT\n");
        free(s_window);
        free(s_fft_cfg);
        return -1;
    }

    /*
     * Précalcul de la fenêtre de Hann
     * Permet de réduire le leakage spectral (artefacts en bords de trame)
     */
    for (i = 0; i < FFT_SIZE; i++) {
        s_window[i] = 0.5f * (1.0f - cosf(2.0f * (float)M_PI * i / (FFT_SIZE - 1)));
    }

    printf("[fft_engine] Init OK — %d points, resolution %.1f Hz/bin\n",
        FFT_SIZE, (float)PLAYER_SAMPLE_RATE / FFT_SIZE);

    return 0;
}

/**
 * @brief Libère la mémoire allouée par fft_engine_init
 */
void fft_engine_cleanup(void)
{
    if (s_fft_out != NULL) free(s_fft_out);
    if (s_window != NULL)  free(s_window);
    if (s_fft_cfg != NULL) free(s_fft_cfg);

    s_fft_out = NULL;
    s_window  = NULL;
    s_fft_cfg = NULL;
}

/**
 * @brief Calcule le spectre de fréquences à partir d'échantillons PCM
 *
 * @param samples       Tableau de FFT_SIZE échantillons float [-1.0, 1.0]
 * @param magnitudes    Tableau de FFT_BINS magnitudes en sortie
 */
void fft_engine_compute(const float *samples, float *magnitudes)
{
    float windowed[FFT_SIZE];
    float re, im;
    int i;

    /* Application de la fenêtre de Hann avant la FFT */
    for (i = 0; i < FFT_SIZE; i++) {
        windowed[i] = samples[i] * s_window[i];
    }

    kiss_fftr(s_fft_cfg, windowed, s_fft_out);

    /* Calcul de la magnitude pour chaque bin : sqrt(re^2 + im^2) */
    for (i = 0; i < FFT_BINS; i++) {
        re = s_fft_out[i].r;
        im = s_fft_out[i].i;
        magnitudes[i] = sqrtf(re * re + im * im);
    }
}
