/**
 * @file ui_midi.h
 * @brief Composant éditeur MIDI : quatre pistes verticales et curseur temporel.
 *
 * Affiche quatre lignes verticales représentant des pistes d'enregistrement.
 * Sur chaque piste des cercles indiquent les appuis enregistrés, et une ligne
 * horizontale mobile (« trotteuse ») visualise la position courante dans la
 * boucle.
 *
 * L'axe temporel va de haut en bas : la valeur 0 correspond au bord supérieur
 * du composant et 1000 au bord inférieur.
 *
 * @version 0.1
 * @date 2026-03-22
 */

#ifndef UI_MIDI_H
#define UI_MIDI_H

#include "ui_types.h"

/**
 * @brief Dessine l'éditeur MIDI à la position et aux dimensions indiquées.
 *
 * Chaque piste est représentée par une ligne verticale. Elle est dessinée dans
 * une couleur atténuée lorsqu'elle est désactivée (@p lineStates[i] == 0).
 *
 * Les cercles sont placés sur la ligne verticale de la piste à une ordonnée
 * proportionnelle à leur champ @c time dans [0, 1000]. Le nom du son est
 * affiché au-dessus de chaque cercle.
 *
 * La ligne curseur est une ligne horizontale pleine à une ordonnée
 * proportionnelle à @p cursorPos dans [0, 1000].
 *
 * @param ctx          Contexte de rendu SDL.
 * @param x            Abscisse du coin supérieur gauche du composant.
 * @param y            Ordonnée du coin supérieur gauche du composant.
 * @param w            Largeur du composant en pixels.
 * @param h            Hauteur du composant en pixels.
 * @param lineStates   Tableau de @c UI_MIDI_TRACK_COUNT entiers : non-zéro =
 *                     piste activée, 0 = piste désactivée.
 * @param presses      Tableau de @c UI_MIDI_TRACK_COUNT pointeurs vers des
 *                     tableaux de @c uiRecordedPress. Peut être NULL.
 * @param pressCounts  Tableau de @c UI_MIDI_TRACK_COUNT entiers indiquant le
 *                     nombre d'éléments dans chaque sous-tableau de @p presses.
 * @param cursorPos    Position courante du curseur dans [0, 1000].
 */
void uiMidi_draw(uiContext_t *ctx,
                 int x, int y, int w, int h,
                 const int            lineStates[],
                 const uiRecordedPress *const presses[],
                 const int            pressCounts[],
                 int                  cursorPos,
                 int                  recordFlag);

#endif /* UI_MIDI_H */
