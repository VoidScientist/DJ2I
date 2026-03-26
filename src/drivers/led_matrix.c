/**
 * @file led_matrix.c
 * @author Louis A. - Mathieu M.
 * @brief Fichier d'implémentation de l'API de la matrice de led.
 * @version 0.1
 * @date 2026-03-22
 * 
 */


#ifndef SIMULATED
    #include <stdio.h>
    #include <string.h>
    #include <wiringPi/wiringPi.h>
#else
    #include <stdio.h>
    #include <string.h>
#endif

#include "drivers/led_matrix.h"


#ifndef SIMULATED
    #define MOSI	10
    #define CS		7
    #define CLK		11

    #define MESS_LEN 16
#endif


#define FIRST_ROW_REG 0x01
#define SHUTDOWN_REG 0x0C
#define BDECODE_REG  0X09
#define BRIGHT_REG   0x0A
#define EN_ROWS_REG  0X0B



#define ROWS 8
#define COLS 8
#define LEDS_AMOUNT 64


static short createMessage(unsigned char addr, unsigned char data);
static void sendMessage(unsigned char addr, unsigned char data);
static void displayArray(const unsigned char *array, int rows, int cols, const char *label);


static unsigned char buffer[LEDS_AMOUNT];
static unsigned char currentState[LEDS_AMOUNT];

static int wasSetup = 0;


void DMATRIX_setupMatrix() {

#ifndef SIMULATED

    printf("Mode du pilote de la matrice de leds: HARDWARE.\n");

	pinMode(MOSI, OUTPUT);
	pinMode(CLK, OUTPUT);
	pinMode(CS, OUTPUT);

	digitalWrite(CS, HIGH);
	digitalWrite(CLK, LOW);
	digitalWrite(MOSI, LOW);

    wasSetup = 1;

	DMATRIX_setOperationMode(M_ON);
	DMATRIX_setBrightness(M_VERY_DIM);
	DMATRIX_disableBDecode();
	DMATRIX_enableAllRows();

#else

    printf("Mode du pilote de la matrice de leds: SIMULATION.\n");
    wasSetup = 1;


#endif
   

}



void DMATRIX_setRow(unsigned char row, unsigned char data) {

    int i;
    int idx = row * COLS;

	if (row >= ROWS) {
        fprintf(stderr, "Mauvaise rangée: %d.\n", row);
        return;
    }

    for (i = 0; i < COLS; i++) {

        // bit de poids fort en premier
        buffer[idx + i] = (data >> (COLS - 1 - i)) & 1;

    }

	// sendMessage(FIRST_ROW_REG + row, data);

}


void DMATRIX_setColumn(unsigned char col, unsigned char data) {

    int i;
    int idx = col;

    if (col >= COLS) {
        fprintf(stderr, "Mauvaise colonne: %d.\n", col);
        return;
    }

    for (i = 0; i < ROWS; i++) {

        // bit de poids fort en premier
        buffer[idx + i * COLS] = (data >> (ROWS - 1 - i)) & 1;

    }


}


void DMATRIX_setOperationMode(powerState_t mode) {
	sendMessage(SHUTDOWN_REG, mode); 
}


void DMATRIX_disableBDecode() {
	sendMessage(BDECODE_REG, 0x00); // on désactive le B-decode
}


void DMATRIX_enableNRows(unsigned char rows) {

	if (rows >= ROWS) rows = ROWS - 1;

	sendMessage(EN_ROWS_REG, rows);

}


void DMATRIX_setBrightness(brightLevel_t brightness) {
	switch (brightness) {
		case M_VERY_DIM: sendMessage(BRIGHT_REG, 0x02); break;
		case M_DIM: sendMessage(BRIGHT_REG, 0x05); break;
		case M_BRIGHT: sendMessage(BRIGHT_REG, 0x08); break;
		case M_VERY_BRIGHT: sendMessage(BRIGHT_REG, 0x0B); break;
		default: sendMessage(BRIGHT_REG, 0x08); break;
	}
}


void DMATRIX_renderBuffer() {

    int i, j;

    for (i = 0; i < ROWS; i++) {

        unsigned char val = 0;

        for (j = 0; j < COLS; j++) {
            val |= buffer[i * COLS + j] << (COLS - 1 - j);
        }

        sendMessage(FIRST_ROW_REG + i, val);

    }

    memcpy(currentState, buffer, sizeof(currentState));

#ifdef SIMULATED
    displayArray(currentState, ROWS, COLS, "Résultat de l'affichage");
#endif

}



void DMATRIX_clearBuffer() {

    memset(buffer, 0, sizeof(buffer));

}

void DMATRIX_clearMatrix() {

    DMATRIX_clearBuffer();
    DMATRIX_renderBuffer();

}


inline void DMATRIX_enableAllRows() { DMATRIX_enableNRows(255); }


static void sendMessage(unsigned char addr, unsigned char data) {

#ifndef SIMULATED

	int i, currentBit;

	short message = createMessage(addr, data);

    if (!wasSetup) {
        printf("Vous devez initialiser le pilote avec DMATRIX_setupMatrix() avant d'envoyer des messages SPI.\n");
        return;
    }

	digitalWrite(CS, LOW);

	delayMicroseconds(1);

	for (i = 0; i < MESS_LEN; i++) {

		currentBit = ( message >> (MESS_LEN - 1 - i) ) & 1;

		// printf("Bit %d: %d\n", MESS_LEN - i, currentBit);

		digitalWrite(MOSI, currentBit);

		delayMicroseconds(1);

		digitalWrite(CLK, HIGH);
		
		delayMicroseconds(1);

		digitalWrite(CLK, LOW);

		delayMicroseconds(1);

	}

	digitalWrite(CS, HIGH);
	digitalWrite(MOSI, LOW);
	digitalWrite(CLK, LOW);

	delayMicroseconds(1);

#else

    printf("Message envoyé à l'adresse 0x%X et données 0x%X.\n", addr, data);

    switch (addr) {

        case SHUTDOWN_REG: printf("État d'opération mis à jour.\n"); break;
        case BRIGHT_REG: printf("Luminosité mise à jour.\n"); break;
        case EN_ROWS_REG: printf("Activation des rangées mises à jour.\n"); break;
        case BDECODE_REG: printf("Activation du BDECODE mis à jour.\n"); break;
        default: 
            if (addr >= FIRST_ROW_REG && addr < FIRST_ROW_REG + ROWS) {
                printf("Mise à jour de la rangée %d.\n", addr - FIRST_ROW_REG);
            } else {
                fprintf(stderr, "[WARNING] Adresse inconnue: 0x%X.\n", addr); 
            }
            break;

    }

#endif

}


static short createMessage(unsigned char addr, unsigned char data) {

	return (addr << 8) | data;

}


/**
 * @brief Affiche n'importe quel tableau 1D représentant une matrice
 * 
 * @param array  Pointeur vers le tableau 1D
 * @param rows   Nombre de lignes
 * @param cols   Nombre de colonnes
 * @param label  Nom du tableau à afficher (ex: "Buffer")
 *
 * @note Cette fonction a été écrite par IA pour gagner du temps et car elle est triviale.
 */
static void displayArray(const unsigned char *array, int rows, int cols, const char *label) {
    int i, j;
    
    printf("=== %s ===\n", label);

    for (i = 0; i < rows; i++) {
    
        for (j = 0; j < cols; j++) {
    
            printf("%d ", array[i * cols + j]);
    
        }
    
        printf("\n");
    
    }
    
    printf("\n");
    
}