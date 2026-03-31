/**
 * @file ui_font.h
 * @brief Rendu de texte bitmap 8×8 sans dépendance SDL_ttf.
 *
 * Expose des fonctions permettant de dessiner du texte à l'écran en utilisant
 * uniquement SDL_RenderDrawPoint, à partir d'une police bitmap intégrée
 * couvrant les caractères ASCII imprimables (0x20–0x7E).
 *
 * @version 0.1
 * @date 2026-03-22
 */

#ifndef UI_FONT_H
#define UI_FONT_H

#include <SDL2/SDL.h>
#include "ui_types.h"

/**
 * @brief Dessine un unique caractère ASCII à la position donnée.
 *
 * Le caractère est rendu en utilisant la police bitmap interne 8×8 multipliée
 * par le facteur @p scale. Les pixels allumés sont dessinés avec la couleur
 * courante du renderer (SDL_SetRenderDrawColor).
 *
 * @param ctx   Contexte de rendu SDL.
 * @param c     Caractère ASCII à dessiner (imprimable, 0x20–0x7E).
 * @param x     Abscisse du coin supérieur gauche du caractère.
 * @param y     Ordonnée du coin supérieur gauche du caractère.
 * @param scale Facteur d'agrandissement (1 = 8px, 2 = 16px, …).
 */
void uiFont_drawChar(uiContext_t *ctx, char c, int x, int y, int scale);

/**
 * @brief Dessine une chaîne de caractères ASCII à la position donnée.
 *
 * Les caractères sont alignés horizontalement, séparés de @p scale × 8 pixels.
 * Les caractères hors de la plage imprimable sont ignorés (remplacés par un
 * espace).
 *
 * @param ctx   Contexte de rendu SDL.
 * @param text  Chaîne de caractères terminée par '\0'.
 * @param x     Abscisse du coin supérieur gauche du premier caractère.
 * @param y     Ordonnée du coin supérieur gauche du premier caractère.
 * @param scale Facteur d'agrandissement appliqué à chaque caractère.
 */
void uiFont_drawText(uiContext_t *ctx, const char *text, int x, int y, int scale);

/**
 * @brief Calcule la largeur en pixels d'une chaîne selon l'échelle donnée.
 *
 * Utile pour centrer un texte horizontalement avant de l'afficher.
 *
 * @param text  Chaîne de caractères terminée par '\0'.
 * @param scale Facteur d'agrandissement.
 * @return      Largeur totale en pixels.
 */
int uiFont_textWidth(const char *text, int scale);

/**
 * @brief Dessine un texte centré horizontalement dans une zone donnée.
 *
 * @param ctx   Contexte de rendu SDL.
 * @param text  Chaîne de caractères terminée par '\0'.
 * @param x     Abscisse du bord gauche de la zone de centrage.
 * @param y     Ordonnée du coin supérieur de la zone de centrage.
 * @param w     Largeur de la zone de centrage en pixels.
 * @param scale Facteur d'agrandissement appliqué à chaque caractère.
 */
void uiFont_drawTextCentered(uiContext_t *ctx, const char *text,
                              int x, int y, int w, int scale);

#endif /* UI_FONT_H */
