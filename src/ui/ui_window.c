/**
 * @file ui_window.c
 * @brief Implémentation de la gestion de la fenêtre SDL.
 *
 * @version 0.1
 * @date 2026-03-22
 */

#include "ui/ui_window.h"
#include "ui/ui_theme.h"
#include <SDL2/SDL.h>
#include <stdio.h>

int uiWindow_init(uiContext_t *ctx, const char *title,
                  int x, int y, int w, int h)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "[ui] SDL_Init error: %s\n", SDL_GetError());
        return -1;
    }

    ctx->width  = w;
    ctx->height = h;

    ctx->window = SDL_CreateWindow(
        title,
        x, y,
        w, h,
        SDL_WINDOW_SHOWN
    );
    if (!ctx->window) {
        fprintf(stderr, "[ui] SDL_CreateWindow error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    ctx->renderer = SDL_CreateRenderer(
        ctx->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!ctx->renderer) {
        /* Repli sur le renderer logiciel si l'accélération est indisponible. */
        ctx->renderer = SDL_CreateRenderer(ctx->window, -1,
                                           SDL_RENDERER_SOFTWARE);
    }
    if (!ctx->renderer) {
        fprintf(stderr, "[ui] SDL_CreateRenderer error: %s\n", SDL_GetError());
        SDL_DestroyWindow(ctx->window);
        ctx->window = NULL;
        SDL_Quit();
        return -1;
    }

    return 0;
}

void uiWindow_destroy(uiContext_t *ctx)
{
    if (ctx->renderer) {
        SDL_DestroyRenderer(ctx->renderer);
        ctx->renderer = NULL;
    }
    if (ctx->window) {
        SDL_DestroyWindow(ctx->window);
        ctx->window = NULL;
    }
    SDL_Quit();
}

void uiWindow_clear(uiContext_t *ctx)
{
    SDL_SetRenderDrawColor(ctx->renderer,
                           UI_COLOR_BG_R,
                           UI_COLOR_BG_G,
                           UI_COLOR_BG_B,
                           UI_COLOR_BG_A);
    SDL_RenderClear(ctx->renderer);
}

void uiWindow_present(uiContext_t *ctx)
{
    SDL_RenderPresent(ctx->renderer);
}
