/**
 * @file ui_midi.c
 * @brief Implémentation du composant éditeur MIDI.
 *
 * @version 0.1
 * @date 2026-03-22
 */

#include "ui/ui_midi.h"
#include "ui/ui_theme.h"
#include "ui/ui_font.h"
#include <SDL2/SDL.h>

/* =========================================================================
 * Fonctions internes
 * ========================================================================= */

/**
 * @brief Convertit une valeur de tick [0, 1000] en ordonnée pixel.
 *
 * @param tick   Valeur temporelle dans [0, 1000].
 * @param originY Ordonnée du bord supérieur du composant.
 * @param height  Hauteur totale du composant en pixels.
 * @return        Ordonnée pixel correspondante.
 */
static int s_tickToY(int tick, int originY, int height)
{
    return originY + (tick * height) / UI_MIDI_TICK_MAX;
}

/**
 * @brief Calcule l'abscisse du centre de la piste @p trackIdx.
 *
 * Les quatre pistes sont réparties uniformément sur la largeur du composant.
 *
 * @param trackIdx Index de la piste (0–3).
 * @param originX  Abscisse du bord gauche du composant.
 * @param width    Largeur totale du composant en pixels.
 * @return         Abscisse du centre de la piste.
 */
static int s_trackCenterX(int trackIdx, int originX, int width)
{
    /* Divise la largeur en UI_MIDI_TRACK_COUNT segments égaux. */
    int segW = width / UI_MIDI_TRACK_COUNT;
    return originX + segW * trackIdx + segW / 2;
}

/**
 * @brief Dessine la bordure et le fond de l'éditeur MIDI.
 *
 * @param ctx  Contexte de rendu SDL.
 * @param x    Abscisse du coin supérieur gauche.
 * @param y    Ordonnée du coin supérieur gauche.
 * @param w    Largeur en pixels.
 * @param h    Hauteur en pixels.
 */
static void s_drawBackground(uiContext_t *ctx,
                              int x, int y, int w, int h)
{
    /* Fond légèrement différent du fond global. */
    SDL_SetRenderDrawColor(ctx->renderer, 25, 25, 35, 255);
    SDL_Rect bg = {x, y, w, h};
    SDL_RenderFillRect(ctx->renderer, &bg);

    /* Bordure. */
    SDL_SetRenderDrawColor(ctx->renderer,
                           UI_COLOR_MIDI_BORDER_R,
                           UI_COLOR_MIDI_BORDER_G,
                           UI_COLOR_MIDI_BORDER_B,
                           UI_COLOR_MIDI_BORDER_A);
    SDL_RenderDrawRect(ctx->renderer, &bg);
}

/**
 * @brief Dessine une ligne verticale de piste.
 *
 * @param ctx     Contexte de rendu SDL.
 * @param cx      Abscisse du centre de la piste.
 * @param y       Ordonnée du bord supérieur de la piste.
 * @param h       Hauteur de la piste en pixels.
 * @param enabled Non-zéro = piste activée.
 */
static void s_drawTrackLine(uiContext_t *ctx,
                             int cx, int y, int h,
                             int enabled)
{
    if (enabled) {
        SDL_SetRenderDrawColor(ctx->renderer,
                               UI_COLOR_MIDI_LINE_R,
                               UI_COLOR_MIDI_LINE_G,
                               UI_COLOR_MIDI_LINE_B,
                               UI_COLOR_MIDI_LINE_A);
    } else {
        SDL_SetRenderDrawColor(ctx->renderer,
                               UI_COLOR_MIDI_LINE_OFF_R,
                               UI_COLOR_MIDI_LINE_OFF_G,
                               UI_COLOR_MIDI_LINE_OFF_B,
                               UI_COLOR_MIDI_LINE_OFF_A);
    }
    SDL_RenderDrawLine(ctx->renderer, cx, y, cx, y + h);
}

/**
 * @brief Dessine un cercle plein (approximation par rectangles).
 *
 * SDL2 de base ne fournit pas de primitive cercle ; on trace des lignes
 * horizontales en utilisant l'équation du cercle.
 *
 * @param ctx Contexte de rendu SDL.
 * @param cx  Abscisse du centre.
 * @param cy  Ordonnée du centre.
 * @param r   Rayon en pixels.
 */
static void s_drawFilledCircle(uiContext_t *ctx, int cx, int cy, int r)
{
    int dy;
    for (dy = -r; dy <= r; dy++) {
        int dx = (int)SDL_sqrt((double)(r * r - dy * dy));
        SDL_RenderDrawLine(ctx->renderer, cx - dx, cy + dy,
                                          cx + dx, cy + dy);
    }
}

/**
 * @brief Dessine les cercles et noms des appuis enregistrés sur une piste.
 *
 * @param ctx     Contexte de rendu SDL.
 * @param cx      Abscisse du centre de la piste.
 * @param originY Ordonnée du bord supérieur du composant.
 * @param height  Hauteur du composant en pixels.
 * @param presses Tableau d'appuis enregistrés pour cette piste.
 * @param count   Nombre d'éléments dans @p presses.
 */
static void s_drawTrackPresses(uiContext_t *ctx,
                                int cx, int originY, int height,
                                const uiRecordedPress *presses,
                                int count)
{
    int i;
    for (i = 0; i < count; i++) {
        int py = s_tickToY(presses[i].time, originY, height);

        /* Cercle. */
        SDL_SetRenderDrawColor(ctx->renderer,
                               UI_COLOR_MIDI_CIRCLE_R,
                               UI_COLOR_MIDI_CIRCLE_G,
                               UI_COLOR_MIDI_CIRCLE_B,
                               UI_COLOR_MIDI_CIRCLE_A);
        s_drawFilledCircle(ctx, cx, py, UI_MIDI_CIRCLE_RADIUS);

        /* Nom du son au-dessus du cercle. */
        if (presses[i].name && presses[i].name[0] != '\0') {
            SDL_SetRenderDrawColor(ctx->renderer,
                                   UI_COLOR_MIDI_TEXT_R,
                                   UI_COLOR_MIDI_TEXT_G,
                                   UI_COLOR_MIDI_TEXT_B,
                                   UI_COLOR_MIDI_TEXT_A);
            int tw    = uiFont_textWidth(presses[i].name, UI_FONT_SCALE_UI);
            int tx    = cx - tw / 2;
            int ty    = py - UI_MIDI_CIRCLE_RADIUS - UI_FONT_CHAR_H - 2;
            uiFont_drawText(ctx, presses[i].name, tx, ty, UI_FONT_SCALE_UI);
        }
    }
}

/**
 * @brief Dessine la ligne curseur horizontale (trotteuse).
 *
 * @param ctx       Contexte de rendu SDL.
 * @param x         Abscisse du bord gauche du composant.
 * @param w         Largeur du composant en pixels.
 * @param originY   Ordonnée du bord supérieur du composant.
 * @param height    Hauteur du composant en pixels.
 * @param cursorPos Position du curseur dans [0, 1000].
 */
static void s_drawCursor(uiContext_t *ctx,
                          int x, int w,
                          int originY, int height,
                          int cursorPos, int recordFlag)
{
    int cy = s_tickToY(cursorPos, originY, height);

    if (recordFlag) {
        SDL_SetRenderDrawColor(ctx->renderer, 80, 220, 80, 200); 
    } else {
        SDL_SetRenderDrawColor(ctx->renderer,
                           UI_COLOR_MIDI_CURSOR_R,
                           UI_COLOR_MIDI_CURSOR_G,
                           UI_COLOR_MIDI_CURSOR_B,
                           UI_COLOR_MIDI_CURSOR_A);
    }

    int i;
    for (i = 0; i < UI_MIDI_CURSOR_W; i++) {
        SDL_RenderDrawLine(ctx->renderer, x, cy + i, x + w, cy + i);
    }
}

/* =========================================================================
 * API publique
 * ========================================================================= */

void uiMidi_draw(uiContext_t *ctx,
                 int x, int y, int w, int h,
                 const int             lineStates[],
                 const uiRecordedPress *const presses[],
                 const int             pressCounts[],
                 int                   cursorPos,
                 int                   recordFlag)
{
    int i;

    s_drawBackground(ctx, x, y, w, h);

    /* Lignes verticales de piste et leurs appuis. */
    for (i = 0; i < UI_MIDI_TRACK_COUNT; i++) {
        int cx      = s_trackCenterX(i, x, w);
        int enabled = lineStates ? lineStates[i] : 1;

        s_drawTrackLine(ctx, cx, y, h, enabled);

        if (presses && presses[i] && pressCounts && pressCounts[i] > 0) {
            s_drawTrackPresses(ctx, cx, y, h, presses[i], pressCounts[i]);
        }
    }

    /* Curseur temporel. */
    s_drawCursor(ctx, x, w, y, h, cursorPos, recordFlag);
}
