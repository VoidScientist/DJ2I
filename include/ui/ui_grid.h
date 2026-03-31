/**
 * @file ui_grid.h
 * @brief Composant grille 4×4 représentant la matrice de boutons physiques.
 *
 * Affiche seize boutons identifiés de 1 à 16 (ligne par ligne, de gauche à
 * droite). Chaque bouton indique visuellement son état (pressé / relâché) et
 * affiche le nom du son associé.
 *
 * @version 0.1
 * @date 2026-03-22
 */

#ifndef UI_GRID_H
#define UI_GRID_H

#include "ui_types.h"
#include "drivers/buttons.h"

/** @brief Nombre total de boutons dans la grille (UI_GRID_ROWS × UI_GRID_COLS). */
#define UI_GRID_BTN_COUNT 16

/**
 * @brief Dessine la grille 4×4 à la position indiquée.
 *
 * Pour chaque bouton :
 * - Le fond est assombri si l'état correspondant est @c B_PRESSED ou @c B_HELD.
 * - Le numéro du bouton (1–16) est centré verticalement dans le cadre.
 * - Le nom du son (@p soundNames) est affiché centré en bas du cadre.
 *   Si @p soundNames ou l'entrée correspondante est NULL, rien n'est affiché.
 *
 * @param ctx        Contexte de rendu SDL.
 * @param x          Abscisse du coin supérieur gauche de la grille.
 * @param y          Ordonnée du coin supérieur gauche de la grille.
 * @param states     Tableau de @c UI_GRID_BTN_COUNT états de boutons,
 *                   indexé ligne-par-ligne (index = row*4 + col).
 * @param soundNames Tableau de @c UI_GRID_BTN_COUNT pointeurs vers les noms
 *                   des sons. Peut être NULL ou contenir des entrées NULL.
 */
void uiGrid_draw(uiContext_t *ctx, int x, int y,
                 const buttonState_t *states,
                 const char *const soundNames[UI_GRID_BTN_COUNT]);

#endif /* UI_GRID_H */
