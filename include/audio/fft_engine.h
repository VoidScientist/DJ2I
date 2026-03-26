/**
 *	\file		fft_engine.h
 *	\brief		Moteur FFT : fenêtrage de Hann + FFT réelle → spectre de magnitudes
 *	\author		MARTEL Mathieu
 *	\version	1.0
 */

#ifndef FFT_ENGINE_H
#define FFT_ENGINE_H

/*
*****************************************************************************************
 *	\noop		I N C L U D E S
 */
#include <stddef.h>

#include "sdl_player.h"

/*
*****************************************************************************************
 *	\noop		C O N S T A N T E S
 */

/** @brief Taille de la FFT en échantillons (puissance de 2, = PLAYER_FRAME_SIZE) */
#define FFT_SIZE	PLAYER_FRAME_SIZE

/** @brief Nombre de bins utiles issus de la FFT réelle (spectre unilatéral) */
#define FFT_BINS	(FFT_SIZE / 2 + 1)

/*
*****************************************************************************************
 *	\noop		F O N C T I O N S
 */

/**
 * @brief Alloue le plan KissFFT et précalcule la fenêtre de Hann
 *
 * @return 0 en succès, -1 en échec d'allocation
 */
int		fft_engine_init(void);

/**
 * @brief Libère les ressources allouées par fft_engine_init()
 */
void	fft_engine_cleanup(void);

/**
 * @brief Calcule le spectre de magnitudes brutes à partir d'une trame PCM
 *
 * Applique la fenêtre de Hann, exécute la FFT réelle, puis calcule
 * la magnitude (√(re²+im²)) de chaque bin. La sortie n'est pas
 * normalisée : c'est le rôle de spectrum_mapper.
 *
 * @param samples		Entrée : FFT_SIZE échantillons mono float [-1.0, 1.0]
 * @param magnitudes	Sortie : FFT_BINS magnitudes brutes (≥ 0)
 */
void	fft_engine_compute(const float *samples, float *magnitudes);

/**
 * @brief Retourne la résolution fréquentielle en Hz par bin
 *
 * @return Résolution en Hz/bin (= SAMPLE_RATE / FFT_SIZE)
 */
float	fft_engine_bin_resolution(void);

#endif /* FFT_ENGINE_H */