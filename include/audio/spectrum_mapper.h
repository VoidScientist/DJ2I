/**
 *	\file		spectrum_mapper.h
 *	\brief		Mapping spectre FFT → hauteurs de bandes pour l'égaliseur
 *	\author		MARTEL Mathieu
 *	\version	1.0
 */

#ifndef SPECTRUM_MAPPER_H
#define SPECTRUM_MAPPER_H

/*
*****************************************************************************************
 *	\noop		I N C L U D E S
 */
#include <stdint.h>

#include "fft_engine.h"

/*
*****************************************************************************************
 *	\noop		C O N S T A N T E S
 */

/** @brief Nombre de bandes fréquentielles (4 bandes × 2 colonnes = 8 colonnes) */
#define NUM_BANDS			4

/** @brief Hauteur maximale d'une barre (bits 1..7 du registre de colonne) */
#define MAX_HEIGHT			7

/*
*****************************************************************************************
 *	\noop		P A R A M È T R E S   D E   R E N D U
 */

/**
 * @brief Plancher de l'échelle dB (niveau considéré comme silence)
 *
 * -60 dB = 1/1000 de l'amplitude maximale.
 * Descendre à -80 dB pour plus de sensibilité sur les sons très faibles.
 */
#define DB_FLOOR			-60.0f

/**
 * @brief Seuil absolu en dessous duquel une bande est forcée à zéro
 *
 * Évite que le leakage spectral résiduel soit converti en dB et amplifié.
 * Plage conseillée : [0.005, 0.05]
 */
#define NOISE_FLOOR			0.02f

/**
 * @brief Coefficient de décroissance de la crête globale par frame
 *
 * 0.995 ≈ descend de moitié en ~140 frames (~5 s à 25 FPS).
 */
#define PEAK_DECAY			0.995f

/**
 * @brief Décroissance des barres en unités [0..7] par frame
 *
 * 0.2 ≈ tombe de 7 à 0 en ~35 frames (~1.4 s à 25 FPS).
 */
#define GRAVITY_STEP		0.2f

/**
 * @brief Nombre de frames pendant lesquelles le marqueur peak hold reste fixe
 *
 * 30 frames ≈ 1.2 s à 25 FPS avant que le marqueur commence à tomber.
 */
#define PEAK_HOLD_FRAMES	30

/**
 * @brief Décroissance du marqueur peak hold en unités [0..7] par frame
 *
 * Plus lent que GRAVITY_STEP pour que le marqueur tombe après la barre.
 */
#define PEAK_HOLD_GRAVITY	0.08f

/*
*****************************************************************************************
 *	\noop		S T R U C T U R E S
 */

/**
 * @brief Résultat du calcul pour une frame complète
 *
 * Contient à la fois les hauteurs de barres et les positions des
 * marqueurs peak hold, pour que matrix_emulator puisse les afficher
 * distinctement.
 */
typedef struct {
	uint8_t	heights[NUM_BANDS];		/**< Hauteur courante de chaque barre [0, MAX_HEIGHT] */
	uint8_t	peaks[NUM_BANDS];		/**< Position du marqueur peak hold  [0, MAX_HEIGHT] */
} BandFrame_t;

/*
*****************************************************************************************
 *	\noop		F O N C T I O N S
 */

/**
 * @brief Initialise les accumulateurs internes
 *
 * @return 0 toujours (pas d'allocation dynamique dans ce module)
 */
int		spectrum_mapper_init(void);

/**
 * @brief Remet à zéro les accumulateurs (utile entre deux fichiers audio)
 */
void	spectrum_mapper_reset(void);

/**
 * @brief Convertit un spectre de magnitudes en hauteurs de barres + peak hold
 *
 * @param magnitudes	Entrée  : FFT_BINS valeurs brutes issues de fft_engine
 * @param frame			Sortie  : hauteurs et positions peak hold
 */
void	spectrum_mapper_compute(const float *magnitudes, BandFrame_t *frame);

#endif /* SPECTRUM_MAPPER_H */