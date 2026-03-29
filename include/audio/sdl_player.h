/**
 *	\file		sdl_player.h
 *	\brief		Gestion de la lecture audio avec SDL2
 *	\author		MARTEL Mathieu - ARCELON Louis
 *	\version	2.0
 */

#ifndef SDL_PLAYER_H
#define SDL_PLAYER_H

/*
*****************************************************************************************
 *	\noop		I N C L U D E S
 */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <SDL2/SDL_mixer.h>

/*
*****************************************************************************************
 *	\noop		C O N S T A N T E S
 */

/** @brief Fréquence d'échantillonnage en Hz */
#define PLAYER_SAMPLE_RATE	44100

/** @brief Nombre de canaux audio (stéréo) */
#define PLAYER_CHANNELS		2

/** @brief Taille du buffer SDL en nombre de frames */
#define PLAYER_CHUNK_SIZE	512

/** @brief Taille d'une trame FFT en échantillons mono */
#define PLAYER_FRAME_SIZE	1024

/*
*****************************************************************************************
 *	\noop		F O N C T I O N S
 */

/**
 * @brief Initialise SDL2 et SDL_mixer, branche le callback audio
 *
 * Opération coûteuse à appeler une seule fois au démarrage.
 * Ne lance aucun son : utiliser sdl_player_play() ensuite.
 *
 * @return 0 si ok, -1 si erreur
 */
int		sdl_player_init(void);

/**
 * @brief Charge et joue un fichier WAV sur un canal libre
 *
 * Peut être appelé plusieurs fois après sdl_player_init().
 * Si un son est déjà en cours, les deux sons sont mixés et
 * capturés ensemble par le pipeline FFT.
 *
 * @param wav_path		Chemin vers le fichier .wav à jouer
 *
 * @return 0 si ok, -1 si erreur
 */
int		sdl_player_play(const char *wav_path);

/**
 * @brief Libère les ressources SDL
 */
void	sdl_player_cleanup(void);

/**
 * @brief Vérifie si au moins un son est en cours de lecture
 *
 * @return true si un son joue encore
 */
bool	sdl_player_is_playing(void);

/**
 * @brief Récupère les derniers échantillons audio dans le buffer
 *
 * @param out		Buffer de sortie (float normalisé entre -1 et 1)
 * @param count		Nombre d'échantillons à récupérer
 *
 * @return true si assez d'échantillons disponibles
 */
bool	sdl_player_read_frame(float *out, size_t count);


int sdl_player_play_chunk(Mix_Chunk *chunk);

#endif /* SDL_PLAYER_H */