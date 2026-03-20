/**
 * Fichier expérimental pour le pilote de la matrice LED. => servira de base pour l'API.
 *
 * Utilise le protocole SPI pour communiquer avec le MAX7219 qui gère la matrice.
 *
 */

#include <stdio.h>
#include <wiringPi.h>

#define MOSI	10
#define CS		7
#define CLK		11

#define MESS_LEN 16

#define SHUTDOWN_REG 0x0C
#define BDECODE_REG  0X09
#define BRIGHT_REG   0x0A
#define EN_ROWS_REG  0X0B

#define ROWS 8

typedef enum {M_OFF, M_ON} powerState_t;
typedef enum {M_VERY_DIM, M_DIM, M_BRIGHT, M_VERY_BRIGHT} brightLevel_t;

short createMessage(unsigned char addr, unsigned char data);
void sendMessage(unsigned char addr, unsigned char data);
void setRow(unsigned char row, unsigned char data);

void setOperationMode(powerState_t mode);
void setBrightness(brightLevel_t level);
void enableNRows(unsigned char maxRow);
void enableAllRows();
void disableBDecode();

void matrixSetup();


int main() {

	int i;

	wiringPiSetupGpio();
	
	// initialise la matrice
	// sendMessage(0x0B, 0x07); // on affiche toutes les rangées. (de 0x00 pour 1 rangée, à 0x07 pour les 8 rangées.)
	matrixSetup();

	for (i = 0; i < 4; i++) {

		setRow(i * 2, 0b11001100);
		setRow(i * 2 + 1, 0b00110011);

	}

	delay(5000);

	setBrightness(M_VERY_DIM);

	delay(2000);

	setOperationMode(M_OFF);


	return 0;

}


void setRow(unsigned char row, unsigned char data) {
	
	if (row >= ROWS) return;

	sendMessage(0x01 + row, data);


}



void sendMessage(unsigned char addr, unsigned char data) {

	int i, currentBit;

	short message = createMessage(addr, data);

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

}


short createMessage(unsigned char addr, unsigned char data) {

	return (addr << 8) + data;

}



void matrixSetup() {

	pinMode(MOSI, OUTPUT);
	pinMode(CLK, OUTPUT);
	pinMode(CS, OUTPUT);

	digitalWrite(CS, HIGH);
	digitalWrite(CLK, LOW);
	digitalWrite(MOSI, LOW);
	

	setOperationMode(M_ON);
	setBrightness(M_BRIGHT);
	disableBDecode();
	enableAllRows();

}


void setOperationMode(powerState_t mode) {
	sendMessage(SHUTDOWN_REG, mode); // mode allumé
}


void disableBDecode() {
	sendMessage(BDECODE_REG, 0x00); // on désactive le B-decode
}


void enableNRows(unsigned char rows) {

	if (rows >= ROWS) rows = ROWS - 1;

	sendMessage(EN_ROWS_REG, rows);

}

inline void enableAllRows() { 
	enableNRows(ROWS - 1);
}

void setBrightness(brightLevel_t brightness) {
	switch (brightness) {
		case M_VERY_DIM: sendMessage(BRIGHT_REG, 0x02); break;
		case M_DIM: sendMessage(BRIGHT_REG, 0x05); break;
		case M_BRIGHT: sendMessage(BRIGHT_REG, 0x08); break;
		case M_VERY_BRIGHT: sendMessage(BRIGHT_REG, 0x0B); break;
	}
}
