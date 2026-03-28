/**
 *	\file		datastructs.h
 *	\brief		Structures de données échangées entre le PC et le Joy-Pi
 *	\author		MARTEL Mathieu
 *	\version	1.0
 */

#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

/*
*****************************************************************************************
 *	\noop		I N C L U D E S
 */
#include <stdint.h>
#include <drivers/buttons.h>
/*
*****************************************************************************************
 *	\noop		C O N S T A N T E S
 */

/** @brief Nombre de colonnes de la matrice LED */
#define NB_COLUMNS		8

/** @brief Nombre maximum de boutons sur le Joy-Pi */
#define MAX_BUTTONS		16

/*
*****************************************************************************************
 *	\noop		S T R U C T U R E S
 */

/**
 * @brief Données spectre à afficher sur la matrice (PC -> Joy-Pi)
 *
 * Contient les 8 registres de colonnes calculés par matrix_emulator_update()
 * côté PC. Chaque registre uint8_t encode les LEDs allumées d'une colonne
 * (bit 0 toujours à 1, bits 1..h allumés selon la hauteur de la barre).
 */
typedef struct {
	uint8_t columns[NB_COLUMNS];
} SpectrumData_t;

/*
*****************************************************************************************
 *	\noop		F O N C T I O N S
 */


void buttonMap2str(buttonStateMap_t event, char *str);


void str2buttonMap(char *str, buttonStateMap_t event);

/**
 * @brief Sérialise un SpectrumData_t en chaîne hexadécimale
 *
 * Format : "0F,0F,1F,1F,07,07,03,03" (8 octets en hex séparés par des virgules)
 *
 * @param data		Données à sérialiser
 * @param str		Buffer de sortie
 */
void spectrumData2str(SpectrumData_t *data, char *str);

/**
 * @brief Désérialise une chaîne hexadécimale en SpectrumData_t
 *
 * @param str		Chaîne à désérialiser
 * @param data		Structure à remplir
 */
void str2spectrumData(char *str, SpectrumData_t *data);

#endif /* DATASTRUCTS_H */