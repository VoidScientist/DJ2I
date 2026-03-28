/**
 * @file buttons.h
 * @author Louis ARCELON - Mathieu MARTEL
 * @brief Fichier en tête contenant l'API externe pour la matrice de boutons
 * @version 0.1
 * @date 2026-03-22
 * 
 */

#ifndef DBUTTON_H
#define DBUTTON_H

#define BUTTON_AMOUNT 16

typedef enum {B_IDLE, B_RELEASED, B_PRESSED, B_HELD} buttonState_t;
typedef buttonState_t buttonStateMap_t[BUTTON_AMOUNT];

void DBUTTON_setupButtons();

void DBUTTON_displayButtonMap();

int DBUTTON_scanButtons();

void DBUTTON_getButtonMap(buttonState_t* result, int size);

buttonState_t DBUTTON_getButtonState(int row, int column);

int DBUTTON_isPressed(int row, int column);

int DBUTTON_isJustPressed(int row, int column);

int DBUTTON_isJustReleased(int row, int column);

#endif /* DBUTTON_H */