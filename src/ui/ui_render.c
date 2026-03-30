/**
 * @file ui_render.c
 * @brief Orchestration du rendu d'une frame complète de l'interface.
 *
 * @version 0.1
 * @date 2026-03-22
 */

#include "ui/ui_render.h"
#include "ui/ui_theme.h"
#include "ui/ui_font.h"
#include "ui/ui_window.h"
#include "ui/ui_grid.h"
#include "ui/ui_midi.h"
#include <SDL2/SDL.h>

/* =========================================================================
 * Calcul de la mise en page
 * ========================================================================= */

/**
 * @brief Largeur totale de la grille 4×4 en pixels (boutons + espacements).
 */
#define GRID_TOTAL_W \
    (UI_GRID_COLS * UI_GRID_BTN_SIZE + (UI_GRID_COLS + 1) * UI_GRID_PADDING)

/**
 * @brief Hauteur totale de la grille 4×4 en pixels.
 */
#define GRID_TOTAL_H \
    (UI_GRID_ROWS * UI_GRID_BTN_SIZE + (UI_GRID_ROWS + 1) * UI_GRID_PADDING)

/** @brief Hauteur de la zone titre (titre + sous-titre + marges). */
#define HEADER_H 80

#define SECTION_GAP 20

/** @brief Hauteur de l'éditeur MIDI en pixels. */
#define MIDI_H 300

#define MIDI_W 500

/* =========================================================================
 * Fonctions internes
 * ========================================================================= */

/**
 * @brief Dessine le titre principal (PROJECT_NAME) centré en haut.
 *
 * @param ctx Contexte de rendu SDL.
 */
static void s_drawTitle(uiContext_t *ctx)
{
    SDL_SetRenderDrawColor(ctx->renderer,
                           UI_COLOR_TITLE_R,
                           UI_COLOR_TITLE_G,
                           UI_COLOR_TITLE_B,
                           UI_COLOR_TITLE_A);

    int scale = UI_FONT_SCALE_TITLE;
    int ty    = 12;
    uiFont_drawTextCentered(ctx, PROJECT_NAME, 0, ty, ctx->width, scale);
}

/**
 * @brief Dessine le sous-titre (VERSION) centré sous le titre.
 *
 * @param ctx Contexte de rendu SDL.
 */
static void s_drawSubtitle(uiContext_t *ctx)
{
    SDL_SetRenderDrawColor(ctx->renderer,
                           UI_COLOR_SUBTITLE_R,
                           UI_COLOR_SUBTITLE_G,
                           UI_COLOR_SUBTITLE_B,
                           UI_COLOR_SUBTITLE_A);

    int scale = UI_FONT_SCALE_SUB;
    int ty    = 12 + UI_FONT_CHAR_H * UI_FONT_SCALE_TITLE + 8;
    uiFont_drawTextCentered(ctx, VERSION, 0, ty, ctx->width, scale);
}

/**
 * @brief Calcule l'abscisse permettant de centrer la grille horizontalement.
 *
 * @param ctx Contexte de rendu SDL.
 * @return    Abscisse du coin supérieur gauche de la grille.
 */
static int s_gridX(uiContext_t *ctx)
{
    return (ctx->width - GRID_TOTAL_W) / 2;
}

/**
 * @brief Calcule l'ordonnée du haut de la grille.
 *
 * @return Ordonnée du coin supérieur gauche de la grille.
 */
static int s_gridY(void)
{
    return HEADER_H;
}

/* =========================================================================
 * API publique
 * ========================================================================= */

void uiRender_frame(uiContext_t *ctx,
                    const buttonState_t      *buttonStates,
                    const char *const         soundNames[UI_GRID_BTN_COUNT],
                    const int                 lineStates[],
                    const uiRecordedPress    *const presses[],
                    const int                 pressCounts[],
                    int                       cursorPos,
                    int                       recordFlag)
{
    /* 1. Effacement. */
    uiWindow_clear(ctx);

    /* 2. Titre. */
    s_drawTitle(ctx);

    /* 3. Sous-titre. */
    s_drawSubtitle(ctx);

    int hpadding = 20;

    int half_w = ctx->width / 2;

    int mw = half_w;   // marge de 20px de chaque côté
    int mh = GRID_TOTAL_H;  // même hauteur que la grille
    
    int gx = (ctx->width - GRID_TOTAL_W - mw - hpadding) / 2;
    int gy = HEADER_H + (ctx->height - HEADER_H - GRID_TOTAL_H) / 2;
    
    int mx = gx + GRID_TOTAL_W + (half_w - mw) / 2 + hpadding;
    int my = gy;             // même ordonnée que la grille

    uiGrid_draw(ctx, gx, gy, buttonStates, soundNames);

                                // ← même ordonnée
    uiMidi_draw(ctx,
                mx, my,
                mw, mh,
                lineStates,
                presses, pressCounts,
                cursorPos, recordFlag);

    /* 6. Présentation. */
    uiWindow_present(ctx);
}
