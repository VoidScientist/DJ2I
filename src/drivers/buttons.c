/**
 * @file buttons.c
 * @author Louis A. - Mathieu M.
 * @brief Fichier d'implémentation de l'API de la matrice de boutons.
 * @version 0.1
 * @date 2026-03-22
 * 
 */


#ifndef SIMULATED
    #include <stdio.h>
    #include <string.h>
    #include <wiringPi/wiringPi.h>
#else
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>
#endif

#include <drivers/drivers_common.h>
#include "drivers/buttons.h"


#define ROWS 4
#define COLS 4
#define BUTTON_AMOUNT 16


#ifndef SIMULATED
    #define C1	25
    #define C2	26
    #define C3 	19
    #define C4  13 

    #define R1  27
    #define R2  22
    #define R3  5
    #define R4  6
#else
    #define BUFFER_SIZE 256
#endif


typedef buttonState_t buttonStateMap_t[BUTTON_AMOUNT];


static void updateOld();
static int buttonFromCoords(int row, int col);


#ifndef SIMULATED
    static int rowPins[] = {R1, R2, R3, R4};
    static int colPins[] = {C1, C2, C3, C4};
#endif


static buttonStateMap_t map = {B_IDLE};
static buttonStateMap_t old = {B_IDLE};


static int wasSetup = 0;


void DBUTTON_setupButtons() {

    DCOMMON_initWPi();

#ifndef SIMULATED

    int i;

    fprintf(stderr, "Mode du pilote boutons: HARDWARE.\n");

    for (i = 0; i < COLS; i++) {

        pinMode(colPins[i], INPUT);
        pullUpDnControl(colPins[i], PUD_UP);

    }
	
    for (i = 0; i < ROWS; i++) {

        pinMode(rowPins[i], OUTPUT);
        digitalWrite(rowPins[i], HIGH);

    }

#else

    fprintf(stderr, "Mode du pilote boutons: SIMULATION.\n");

#endif

    wasSetup = 1;

}


int DBUTTON_scanButtons() {

#ifndef SIMULATED

	int rowPin = rowPins[0];
	int colPin = colPins[0];
	int colVal = 0;	
	
	int amountPressed = 0;

	int idx, i, j;

    if (wasSetup != 1) {
        fprintf(stderr, "ERREUR: Veuillez initialiser le pilote des boutons avant de pouvoir scanner.\n");
        return -1;
    }

	for (i = 0; i < ROWS; i++) {

		rowPin = rowPins[i];
		digitalWrite(rowPin, LOW);		

		for (j = 0; j < COLS; j++) {

			colPin = colPins[j];
			colVal = digitalRead(colPin);

			idx = i * COLS + j;
			
			if (colVal == LOW) {
				switch (old[idx]) {				
					case B_IDLE: 		map[idx] = B_PRESSED; 	break;
					case B_RELEASED: 	map[idx] = B_PRESSED; 	break;
					default: 			map[idx] = B_HELD; 		break;
				} 
				amountPressed++;
			} else {
				switch(old[idx]) {
					case B_PRESSED:	map[idx] = B_RELEASED; 	break;
					case B_HELD:	map[idx] = B_RELEASED; 	break;
					default:		map[idx] = B_IDLE; 		break;
				}
			}

		}

		digitalWrite(rowPin, HIGH);
		delay(1);

	}

	updateOld();

	return amountPressed;

#else

    int button;
    buttonState_t state;
    char buffer[BUFFER_SIZE];
    char *enumState;

    printf("État actuel: \n");
    DBUTTON_displayButtonMap();

    printf("Choisir un bouton dont l'état est à modifier (0 à %d): ", BUTTON_AMOUNT);
    fgets(buffer, BUFFER_SIZE, stdin);
    button = abs(atoi(buffer)) % BUTTON_AMOUNT;

    printf("Choisir le nouvel état du bouton %d (0 à 3): ", button);
    fgets(buffer, BUFFER_SIZE, stdin);
    state = abs(atoi(buffer)) % 4;

    switch (state) {
        case B_IDLE: enumState = "B_IDLE"; break;
        case B_RELEASED: enumState = "B_RELEASED"; break;
        case B_HELD: enumState = "B_HELD"; break;
        case B_PRESSED: enumState = "B_PRESSED"; break;
    }
    
    map[button] = state;

    printf("Bouton de coordonnées (%d, %d) mis à l'état %s.\n", button / ROWS, button % ROWS, enumState);

    DBUTTON_displayButtonMap();

    return 1;

#endif

}


void DBUTTON_displayButtonMap() {

	int i, idx;
	for (i = 0; i < ROWS; i++) {
		idx = i * COLS;
		printf("%d %d %d %d\n", map[idx], map[idx+1], map[idx+2], map[idx+3]);
	}

}


void DBUTTON_getButtonMap(buttonState_t *result, int size) {

    int cpySize = size <= BUTTON_AMOUNT ? size : BUTTON_AMOUNT;
    memcpy(result, map, cpySize * sizeof(buttonState_t));

}


buttonState_t DBUTTON_getButtonState(int row, int column) {

    int button = buttonFromCoords(row, column);
    return map[button];
    
}

int DBUTTON_isPressed(int row, int column) {
    buttonState_t state = DBUTTON_getButtonState(row, column);
    return state == B_HELD || state == B_PRESSED;
}

int DBUTTON_isJustPressed(int row, int column) {
    buttonState_t state = DBUTTON_getButtonState(row, column);
    return state == B_PRESSED;
}

int DBUTTON_isJustReleased(int row, int column) {
    buttonState_t state = DBUTTON_getButtonState(row, column);
    return state == B_RELEASED;
}


static void updateOld() {
	int i;
	for (i = 0; i < BUTTON_AMOUNT; i++) {
		old[i] = map[i];
	}
}


static int buttonFromCoords(int row, int col) {

    return row * COLS + col;

}