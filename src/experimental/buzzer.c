#include <stdio.h>
#include <wiringPi.h>

#define PIN 18
#define C1  13
#define R1  27
#define R2  22
#define R3  5
#define R4  6

int main() {

	wiringPiSetupGpio();

	pinMode(PIN, OUTPUT);
	pinMode(R1, OUTPUT);
	pinMode(C1, INPUT);

	digitalWrite(R1, HIGH);

	for (;;) {

		printf("State of C1: %d\n", digitalRead(C1));

		if (digitalRead(C1) != 1) {
			printf("Button is high\n");
			digitalWrite(PIN, HIGH);
		} else {
			printf("Button is low\n");
			digitalWrite(PIN, LOW);
		}

		delay(100);

	}

}
