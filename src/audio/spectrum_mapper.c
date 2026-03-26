/**
 *  \file       spectrum_mapper.c
 *  \brief      Implémentation de la conversion spectre -> barres d'égaliseur
 *  \author     MARTEL Mathieu
 *  \version    1.0
 */

/*
*****************************************************************************************
 *  \noop       I N C L U D E S
 */
#include <math.h>
#include <stdio.h>

#include "audio/spectrum_mapper.h"

/*
*****************************************************************************************
 *  \noop       C O N S T A N T E S
 */

/*
 * Fréquences limites des 4 bandes (en Hz)
 * On utilise une échelle logarithmique pour coller à la perception humaine
 *   Bande 0 : Bass    20  - 250  Hz
 *   Bande 1 : Lo-Mid  250 - 2000 Hz
 *   Bande 2 : Hi-Mid  2k  - 6000 Hz
 *   Bande 3 : Treble  6k  - 20k  Hz
 */
static const float BAND_LIMITS[NUM_BANDS + 1] = {
    20.0f, 250.0f, 2000.0f, 6000.0f, 20000.0f
};

/*
*****************************************************************************************
 *  \noop       V A R I A B L E S   G L O B A L E S
 */

/** @brief Valeur maximale vue sur toutes les bandes (sert de référence pour la normalisation) */
static float s_global_peak;

/** @brief Hauteurs actuelles des barres (float pour la gestion de la gravité) */
static float s_bar_heights[NUM_BANDS];

/** @brief Position actuelle des marqueurs peak hold */
static float s_peak_hold[NUM_BANDS];

/** @brief Compteur de frames avant que le marqueur commence à tomber */
static int s_peak_hold_timer[NUM_BANDS];

/*
*****************************************************************************************
 *  \noop       F O N C T I O N S   P U B L I Q U E S
 */

/**
 * @brief Initialise le module
 *
 * @return 0 toujours
 */
int spectrum_mapper_init(void)
{
    int b;

    spectrum_mapper_reset();

    printf("[spectrum_mapper] Bandes :");
    for (b = 0; b < NUM_BANDS; b++)
        printf("  %.0f-%.0fHz", BAND_LIMITS[b], BAND_LIMITS[b + 1]);
    printf("\n");

    return 0;
}

/**
 * @brief Remet les accumulateurs à zéro
 */
void spectrum_mapper_reset(void)
{
    int b;

    s_global_peak = 0.001f;

    for (b = 0; b < NUM_BANDS; b++) {
        s_bar_heights[b]      = 0.0f;
        s_peak_hold[b]        = 0.0f;
        s_peak_hold_timer[b]  = 0;
    }
}

/**
 * @brief Calcule les hauteurs de barres à partir du spectre FFT
 *
 * Pour chaque bande on prend le bin de magnitude maximale (pas la moyenne),
 * ce qui évite que les hautes fréquences paraissent plus faibles juste
 * parce qu'elles ont plus de bins.
 *
 * @param magnitudes    Tableau de FFT_BINS magnitudes (sortie de fft_engine)
 * @param frame         Résultat : hauteurs + positions peak hold
 */
void spectrum_mapper_compute(const float *magnitudes, BandFrame_t *frame)
{
    float energy[NUM_BANDS];
    float frame_max;
    float norm, db, target;
    int bin_lo, bin_hi;
    int b, k, h;

    /* Étape 1 : trouver le pic de chaque bande  */
    for (b = 0; b < NUM_BANDS; b++) {

        /* Conversion fréquence -> indice de bin */
        bin_lo = (int)ceilf(BAND_LIMITS[b] * FFT_SIZE / PLAYER_SAMPLE_RATE);
        bin_hi = (int)floorf(BAND_LIMITS[b + 1] * FFT_SIZE / PLAYER_SAMPLE_RATE);

        if (bin_lo < 0)        bin_lo = 0;
        if (bin_hi >= FFT_BINS) bin_hi = FFT_BINS - 1;

        energy[b] = 0.0f;
        for (k = bin_lo; k <= bin_hi; k++) {
            if (magnitudes[k] > energy[b])
                energy[b] = magnitudes[k];
        }

        /* On ignore les valeurs trop faibles (bruit de fond / leakage) */
        if (energy[b] < NOISE_FLOOR)
            energy[b] = 0.0f;
    }

    /* Étape 2 : mise à jour du pic global (auto-gain) */
    frame_max = 0.0f;
    for (b = 0; b < NUM_BANDS; b++) {
        if (energy[b] > frame_max)
            frame_max = energy[b];
    }

    if (frame_max > s_global_peak) {
        s_global_peak = frame_max;
    } else {
        s_global_peak *= PEAK_DECAY;
        if (s_global_peak < NOISE_FLOOR)
            s_global_peak = NOISE_FLOOR;
    }

    /* Étape 3 : conversion en hauteur + lissage */
    for (b = 0; b < NUM_BANDS; b++) {

        /*
         * Conversion en dB puis normalisation sur [0, MAX_HEIGHT]
         * L'échelle dB est plus adaptée à la perception humaine
         * qu'une simple valeur linéaire ou racine carrée
         */
        if (energy[b] <= 0.0f) {
            target = 0.0f;
        } else {
            norm = energy[b] / s_global_peak;
            if (norm > 1.0f) norm = 1.0f;

            db = 20.0f * log10f(norm);
            if (db < DB_FLOOR) db = DB_FLOOR;

            target = ((db - DB_FLOOR) / (-DB_FLOOR)) * MAX_HEIGHT;
        }

        /* Attaque instantanée, descente progressive (effet VU-mètre) */
        if (target > s_bar_heights[b]) {
            s_bar_heights[b] = target;
        } else {
            s_bar_heights[b] -= GRAVITY_STEP;
            if (s_bar_heights[b] < 0.0f)
                s_bar_heights[b] = 0.0f;
        }

        h = (int)(s_bar_heights[b] + 0.5f);
        if (h > MAX_HEIGHT) h = MAX_HEIGHT;
        frame->heights[b] = (uint8_t)h;

        /* Marqueur peak hold : se bloque au maximum puis tombe lentement */
        if (s_bar_heights[b] >= s_peak_hold[b]) {
            s_peak_hold[b] = s_bar_heights[b];
            s_peak_hold_timer[b] = PEAK_HOLD_FRAMES;
        } else if (s_peak_hold_timer[b] > 0) {
            s_peak_hold_timer[b]--;
        } else {
            s_peak_hold[b] -= PEAK_HOLD_GRAVITY;
            if (s_peak_hold[b] < 0.0f)
                s_peak_hold[b] = 0.0f;
        }

        h = (int)(s_peak_hold[b] + 0.5f);
        if (h > MAX_HEIGHT) h = MAX_HEIGHT;
        if (h < (int)frame->heights[b]) h = (int)frame->heights[b];
        frame->peaks[b] = (uint8_t)h;
    }
}
