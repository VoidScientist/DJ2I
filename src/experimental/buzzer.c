/**
 * Pilote expérimental pour la matrice de bouton => servira de fondation pour l'API.
 *
 * Remarques HW:
 * - Les lignes doivent être LOW pour détecter LOW sur les GPIO des colonnes
 * - Les colonnes sont inversées par rapport à la documentation pour une certaine raison
 *
 * Fonctionnement:
 * - On scanne ligne par ligne et on peuple un tableau passé en argument.
 * - On garde en mémoire l'état précédent des bouttons, afin d'avoir les états PRESSED, RELEASED, HELD, IDLE
 *   	- HELD: appuyé depuis au moins deux scans
 *   	- RELEASED: tout juste relâché 
 *   	- IDLE: n'est pas appuyé
 *   	- PRESSED: tout juste appuyé
 *
 */
#include <stdio.h>
#include <wiringPi.h>

#define PIN 18

#define ROWS 4
#define COLS 4
#define BUTTON_AMOUNT 16

#define C1	25
#define C2	26
#define C3 	19
#define C4  13 

#define R1  27
#define R2  22
#define R3  5
#define R4  6

typedef enum {B_IDLE, B_RELEASED, B_PRESSED, B_HELD} buttonState_t;

typedef buttonState_t buttonStateMap_t[BUTTON_AMOUNT];


int rowPins[] = {R1, R2, R3, R4};
int colPins[] = {C1, C2, C3, C4};
static buttonStateMap_t old = {B_IDLE};


void setupButtons();
void displayButtonMap(buttonStateMap_t map);
void updateOld(buttonStateMap_t map);
int scanButtons(buttonStateMap_t map);


int main() {

	buttonStateMap_t map;

	wiringPiSetupGpio();

	setupButtons();

	for (;;) {

		scanButtons(map);

		printf("Current:\n");		

		displayButtonMap(map);
	
		printf("Old:\n");

		displayButtonMap(old);

		updateOld(map);

		delay(100);

	}

}


void setupButtons() {

	pinMode(C1, INPUT);
	pinMode(C2, INPUT);
	pinMode(C3, INPUT);
	pinMode(C4, INPUT);	

	pullUpDnControl(C1, PUD_UP);
	pullUpDnControl(C2, PUD_UP);
	pullUpDnControl(C3, PUD_UP);
	pullUpDnControl(C4, PUD_UP);

	pinMode(R1, OUTPUT);
	pinMode(R2, OUTPUT);
	pinMode(R3, OUTPUT);
	pinMode(R4, OUTPUT);

	digitalWrite(R1, HIGH);
	digitalWrite(R2, HIGH);
	digitalWrite(R3, HIGH);
	digitalWrite(R4, HIGH);

}


int scanButtons(buttonStateMap_t map) {

	int rowPin = rowPins[0];
	int colPin = colPins[0];
	int colVal = 0;	
	
	int amountPressed = 0;

	int idx, i, j;

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

	return amountPressed;

}


void displayButtonMap(buttonStateMap_t map) {

	int i, idx;
	for (i = 0; i < ROWS; i++) {
		idx = i * ROWS;
		printf("%d %d %d %d\n", map[idx], map[idx+1], map[idx+2], map[idx+3]);
	}

}

void updateOld(buttonStateMap_t map) {
	int i;
	for (i = 0; i < BUTTON_AMOUNT; i++) {
		old[i] = map[i];
	}
}
