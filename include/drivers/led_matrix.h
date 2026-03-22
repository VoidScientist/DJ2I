/**
 * @file led_matrix.h
 * @author Louis A. - Mathieu M.
 * @brief Fichier en-tête contenant l'API externe de la matrice de LED.
 * @version 0.1
 * @date 2026-03-22
 * 
 */

#ifndef DMATRIX_H
#define DMATRIX_H

typedef enum {M_OFF, M_ON} powerState_t;
typedef enum {M_VERY_DIM, M_DIM, M_BRIGHT, M_VERY_BRIGHT} brightLevel_t;


void DMATRIX_setOperationMode(powerState_t mode);
void DMATRIX_setBrightness(brightLevel_t brightness);
void DMATRIX_enableNRows(unsigned char maxRow);
void DMATRIX_disableBDecode();

void DMATRIX_setupMatrix();


void DMATRIX_setRow(unsigned char row, unsigned char data);
void DMATRIX_setColumn(unsigned char col, unsigned char data);

// down from here is not done yet

void DMATRIX_renderBuffer();
void DMATRIX_clearBuffer();

void DMATRIX_clearMatrix();

/**
 * @brief Permet d'activer l'affichage sur toutes les rangées. Utilise le fait que DMATRIX_enableNRows() cappe son argument à ROWS - 1.
 */
void DMATRIX_enableAllRows();

#endif /* DMATRIX_H */