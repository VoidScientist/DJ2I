/**
 * @file ui_render.h
 * @brief Orchestration du rendu d'une frame complète.
 *
 * Expose une unique fonction de haut niveau qui efface l'écran puis dessine
 * successivement le titre, le sous-titre, la grille et l'éditeur MIDI, avant
 * de présenter le résultat à l'écran.
 *
 * @version 0.1
 * @date 2026-03-22
 */

#ifndef UI_RENDER_H
#define UI_RENDER_H

#include "ui_types.h"
#include "drivers/buttons.h"
#include "ui_grid.h"

/**
 * @brief Dessine une frame complète de l'interface looper.
 *
 * Séquence de rendu :
 *  1. Effacement du fond.
 *  2. Titre (PROJECT_NAME) centré en haut de la fenêtre.
 *  3. Sous-titre (VERSION) centré sous le titre.
 *  4. Grille 4×4 centrée horizontalement.
 *  5. Éditeur MIDI centré horizontalement sous la grille.
 *  6. Présentation du frame buffer.
 *
 * @param ctx          Contexte de rendu SDL.
 * @param buttonStates Tableau de @c UI_GRID_BTN_COUNT états de boutons,
 *                     indexé ligne-par-ligne (row*4 + col).
 * @param soundNames   Tableau de @c UI_GRID_BTN_COUNT chaînes de noms de sons.
 *                     Les entrées NULL sont autorisées.
 * @param lineStates   Tableau de @c UI_MIDI_TRACK_COUNT états de pistes MIDI
 *                     (non-zéro = activé).
 * @param presses      Tableau de @c UI_MIDI_TRACK_COUNT pointeurs vers des
 *                     tableaux de @c uiRecordedPress.
 * @param pressCounts  Tableau de @c UI_MIDI_TRACK_COUNT nombres d'éléments
 *                     dans chaque sous-tableau de @p presses.
 * @param cursorPos    Position du curseur MIDI dans [0, 1000].
 */
void uiRender_frame(uiContext_t *ctx,
                    const buttonState_t      *buttonStates,
                    const char *const         soundNames[UI_GRID_BTN_COUNT],
                    const int                 lineStates[],
                    const uiRecordedPress    *const presses[],
                    const int                 pressCounts[],
                    int                       cursorPos);

#endif /* UI_RENDER_H */
