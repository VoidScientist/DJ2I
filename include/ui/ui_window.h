/**
 * @file ui_window.h
 * @brief Initialisation et destruction de la fenêtre SDL.
 *
 * @version 0.1
 * @date 2026-03-22
 */

#ifndef UI_WINDOW_H
#define UI_WINDOW_H

#include "ui_types.h"

/**
 * @brief Initialise SDL, crée la fenêtre et le renderer.
 *
 * Initialise le sous-système vidéo de SDL2, crée une fenêtre à la position et
 * aux dimensions spécifiées, puis crée un renderer avec accélération matérielle
 * si disponible.
 *
 * @param ctx    Pointeur vers le contexte à initialiser.
 * @param title  Titre de la fenêtre (chaîne terminée par '\0').
 * @param x      Position X de la fenêtre (SDL_WINDOWPOS_CENTERED possible).
 * @param y      Position Y de la fenêtre (SDL_WINDOWPOS_CENTERED possible).
 * @param w      Largeur de la fenêtre en pixels.
 * @param h      Hauteur de la fenêtre en pixels.
 * @return       0 en cas de succès, -1 en cas d'erreur SDL.
 */
int uiWindow_init(uiContext_t *ctx, const char *title,
                  int x, int y, int w, int h);

/**
 * @brief Libère le renderer, la fenêtre et arrête SDL.
 *
 * @param ctx Pointeur vers le contexte à détruire.
 */
void uiWindow_destroy(uiContext_t *ctx);

/**
 * @brief Efface le renderer avec la couleur de fond du thème.
 *
 * Doit être appelé en début de chaque frame avant tout dessin.
 *
 * @param ctx Contexte de rendu SDL.
 */
void uiWindow_clear(uiContext_t *ctx);

/**
 * @brief Présente le renderer (swap buffers).
 *
 * Doit être appelé en fin de chaque frame après tout dessin.
 *
 * @param ctx Contexte de rendu SDL.
 */
void uiWindow_present(uiContext_t *ctx);

#endif /* UI_WINDOW_H */
