/**
 * @file ui_types.h
 * @brief Types et structures partagés par les composants de l'interface.
 *
 * @version 0.1
 * @date 2026-03-22
 */

#ifndef UI_TYPES_H
#define UI_TYPES_H

#include <SDL2/SDL.h>

/**
 * @brief Contexte SDL principal de l'interface.
 *
 * Regroupe la fenêtre SDL et le renderer associé. Ce contexte est créé par
 * @ref uiWindow_init et libéré par @ref uiWindow_destroy.
 */
typedef struct {
    SDL_Window   *window;   /**< @brief Pointeur vers la fenêtre SDL. */
    SDL_Renderer *renderer; /**< @brief Pointeur vers le renderer SDL. */
    int           width;    /**< @brief Largeur courante de la fenêtre en pixels. */
    int           height;   /**< @brief Hauteur courante de la fenêtre en pixels. */
} uiContext_t;

/**
 * @brief Représente un appui enregistré à afficher dans l'éditeur MIDI.
 *
 * Chaque instance est positionnée sur une des quatre lignes verticales de
 * l'éditeur et placée verticalement selon sa valeur temporelle.
 */
typedef struct {
    char *name; /**< @brief Nom du son joué, affiché au-dessus du cercle. */
    int   time; /**< @brief Position temporelle dans [0, 1000] sur la ligne. */
} uiRecordedPress;

#endif /* UI_TYPES_H */
