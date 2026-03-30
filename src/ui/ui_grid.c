/**
 * @file ui_grid.c
 * @brief Implémentation du composant grille 4×4.
 *
 * @version 0.1
 * @date 2026-03-22
 */

#include "ui/ui_grid.h"
#include "ui/ui_theme.h"
#include "ui/ui_font.h"
#include <stdio.h>
#include <SDL2/SDL.h>

/* =========================================================================
 * Fonctions internes
 * ========================================================================= */

/**
 * @brief Indique si un état de bouton correspond à un appui actif.
 *
 * @param state État du bouton.
 * @return      1 si pressé ou maintenu, 0 sinon.
 */
static int s_isActive(buttonState_t state)
{
    return (state == B_PRESSED || state == B_HELD) ? 1 : 0;
}

/**
 * @brief Dessine le fond coloré d'un seul bouton.
 *
 * @param ctx     Contexte de rendu SDL.
 * @param rect    Rectangle délimitant le bouton.
 * @param active  Non-zéro = bouton pressé (fond assombri).
 */
static void s_drawButtonBackground(uiContext_t *ctx,
                                   const SDL_Rect *rect,
                                   int active)
{
    if (active) {
        SDL_SetRenderDrawColor(ctx->renderer,
                               UI_COLOR_BTN_PRESSED_R,
                               UI_COLOR_BTN_PRESSED_G,
                               UI_COLOR_BTN_PRESSED_B,
                               UI_COLOR_BTN_PRESSED_A);
    } else {
        SDL_SetRenderDrawColor(ctx->renderer,
                               UI_COLOR_BTN_R,
                               UI_COLOR_BTN_G,
                               UI_COLOR_BTN_B,
                               UI_COLOR_BTN_A);
    }
    SDL_RenderFillRect(ctx->renderer, rect);
}

/**
 * @brief Dessine la bordure d'un bouton.
 *
 * @param ctx  Contexte de rendu SDL.
 * @param rect Rectangle délimitant le bouton.
 */
static void s_drawButtonBorder(uiContext_t *ctx, const SDL_Rect *rect)
{
    SDL_SetRenderDrawColor(ctx->renderer,
                           UI_COLOR_BTN_BORDER_R,
                           UI_COLOR_BTN_BORDER_G,
                           UI_COLOR_BTN_BORDER_B,
                           UI_COLOR_BTN_BORDER_A);

    /* Dessin du contour pixel par pixel (SDL n'a pas SDL_RenderDrawRectW). */
    int i;
    for (i = 0; i < UI_GRID_BORDER_W; i++) {
        SDL_Rect border = {
            rect->x + i,
            rect->y + i,
            rect->w - 2 * i,
            rect->h - 2 * i
        };
        SDL_RenderDrawRect(ctx->renderer, &border);
    }
}

/**
 * @brief Affiche le numéro d'index d'un bouton centré dans le cadre.
 *
 * @param ctx    Contexte de rendu SDL.
 * @param rect   Rectangle du bouton.
 * @param index  Numéro à afficher (1–16).
 */
static void s_drawButtonIndex(uiContext_t *ctx,
                               const SDL_Rect *rect,
                               int index)
{
    char buf[4];
    snprintf(buf, sizeof(buf), "%d", index);

    SDL_SetRenderDrawColor(ctx->renderer,
                           UI_COLOR_BTN_TEXT_R,
                           UI_COLOR_BTN_TEXT_G,
                           UI_COLOR_BTN_TEXT_B,
                           UI_COLOR_BTN_TEXT_A);

    int scale = UI_FONT_SCALE_UI + 1; /* échelle 2 pour le numéro */
    int th    = UI_FONT_CHAR_H * scale;
    int ty    = rect->y + (rect->h - th) / 2;

    uiFont_drawTextCentered(ctx, buf, rect->x, ty, rect->w, scale);
}

/**
 * @brief Affiche le nom du son centré en bas du cadre d'un bouton.
 *
 * @param ctx   Contexte de rendu SDL.
 * @param rect  Rectangle du bouton.
 * @param name  Nom du son (peut être NULL).
 */
static void s_drawButtonSoundName(uiContext_t *ctx,
                                   const SDL_Rect *rect,
                                   const char *name)
{
    if (!name || name[0] == '\0') return;

    SDL_SetRenderDrawColor(ctx->renderer,
                           UI_COLOR_BTN_TEXT_R,
                           UI_COLOR_BTN_TEXT_G,
                           UI_COLOR_BTN_TEXT_B,
                           (Uint8)(UI_COLOR_BTN_TEXT_A * 3 / 4));

    int scale = UI_FONT_SCALE_UI;
    int th    = UI_FONT_CHAR_H * scale;
    /* Marges : 4 px depuis le bas intérieur du bouton. */
    int ty    = rect->y + rect->h - th - UI_GRID_BORDER_W - 4;

    uiFont_drawTextCentered(ctx, name, rect->x, ty, rect->w, scale);
}

/* =========================================================================
 * API publique
 * ========================================================================= */

void uiGrid_draw(uiContext_t *ctx, int x, int y,
                 const buttonState_t *states,
                 const char *const soundNames[UI_GRID_BTN_COUNT])
{
    int row, col;

    for (row = 0; row < UI_GRID_ROWS; row++) {
        for (col = 0; col < UI_GRID_COLS; col++) {
            int idx = row * UI_GRID_COLS + col;

            SDL_Rect rect = {
                x + col * (UI_GRID_BTN_SIZE + UI_GRID_PADDING) + UI_GRID_PADDING,
                y + row * (UI_GRID_BTN_SIZE + UI_GRID_PADDING) + UI_GRID_PADDING,
                UI_GRID_BTN_SIZE,
                UI_GRID_BTN_SIZE
            };

            int active = states ? s_isActive(states[idx]) : 0;

            s_drawButtonBackground(ctx, &rect, active);
            s_drawButtonBorder(ctx, &rect);
            s_drawButtonIndex(ctx, &rect, idx + 1);

            if (soundNames) {
                s_drawButtonSoundName(ctx, &rect, soundNames[idx]);
            }
        }
    }
}
