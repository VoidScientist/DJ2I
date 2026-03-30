/**
 * @file test_ui.c
 * @brief Script de test du module UI looper/séquenceur.
 *
 * Ce programme synthétise un état complet de l'interface et l'affiche
 * pendant quelques secondes. Il est conçu pour être compilé et exécuté sur
 * x86_64 (développement) et ARM/RPI (cible embarquée).
 *
 * Compilation exemple (x86_64) :
 * @code
 *   gcc -I include/ \
 *       tools/test_code/test_ui.c \
 *       src/ui/ui_font.c src/ui/ui_window.c \
 *       src/ui/ui_grid.c src/ui/ui_midi.c src/ui/ui_render.c \
 *       -lSDL2 -lm \
 *       -DPROJECT_NAME='"LooperUI"' -DVERSION='"v0.1-test"' \
 *       -o builds/x86_64/test_ui
 * @endcode
 *
 * @version 0.1
 * @date 2026-03-22
 */

#include "ui/ui.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

#include <drivers/buttons.h>

/* =========================================================================
 * Constantes du test
 * ========================================================================= */

/** @brief Durée totale d'affichage du test en millisecondes. */
#define TEST_DURATION_MS  5000
/** @brief Délai entre deux frames en millisecondes (≈ 60 fps). */
#define TEST_FRAME_MS       16
/** @brief Vitesse d'avancement du curseur MIDI (ticks par frame). */
#define CURSOR_SPEED         3

/* =========================================================================
 * Données de test
 * ========================================================================= */

/**
 * @brief Noms de sons fictifs associés aux 16 boutons.
 */
static const char *s_soundNames[UI_GRID_BTN_COUNT] = {
    "kick",  "snare", "hihat", "clap",
    "bass",  "lead",  "pad",   "perc",
    "fx1",   "fx2",   "atm",   "vox",
    "tom",   "ride",  "crash", "cowbel"
};

/**
 * @brief États des boutons physiques simulés pour le test.
 *
 * Les boutons 0 et 5 sont maintenus pressés pour illustrer l'assombrissement.
 */
static buttonState_t s_buttonStates[UI_GRID_BTN_COUNT] = {
    B_HELD,  B_IDLE, B_IDLE, B_IDLE,
    B_IDLE,  B_HELD, B_IDLE, B_IDLE,
    B_IDLE,  B_IDLE, B_PRESSED, B_IDLE,
    B_PRESSED, B_IDLE, B_IDLE, B_IDLE
};

/**
 * @brief États des quatre pistes MIDI (toutes activées sauf la troisième).
 */
static int s_lineStates[UI_MIDI_TRACK_COUNT] = {1, 1, 0, 1};

/**
 * @brief Appuis enregistrés sur la piste 0.
 */
static uiRecordedPress s_presses0[] = {
    {"kick",  100},
    {"kick",  350},
    {"kick",  600},
    {"kick",  850},
};

/**
 * @brief Appuis enregistrés sur la piste 1.
 */
static uiRecordedPress s_presses1[] = {
    {"snare", 250},
    {"snare", 750},
};

/**
 * @brief Appuis enregistrés sur la piste 2 (piste désactivée dans le test).
 */
static uiRecordedPress s_presses2[] = {
    {"hihat",  50},
    {"hihat", 175},
    {"hihat", 300},
    {"hihat", 425},
    {"hihat", 550},
    {"hihat", 675},
    {"hihat", 800},
    {"hihat", 925},
};

/**
 * @brief Appuis enregistrés sur la piste 3.
 */
static uiRecordedPress s_presses3[] = {
    {"clap",  500},
};

/**
 * @brief Tableau agrégateur des pointeurs de pistes.
 */
static const uiRecordedPress *s_presses[UI_MIDI_TRACK_COUNT] = {
    s_presses0,
    s_presses1,
    s_presses2,
    s_presses3,
};

/**
 * @brief Nombre d'appuis par piste.
 */
static int s_pressCounts[UI_MIDI_TRACK_COUNT] = {
    (int)(sizeof(s_presses0) / sizeof(s_presses0[0])),
    (int)(sizeof(s_presses1) / sizeof(s_presses1[0])),
    (int)(sizeof(s_presses2) / sizeof(s_presses2[0])),
    (int)(sizeof(s_presses3) / sizeof(s_presses3[0])),
};

/* =========================================================================
 * Point d'entrée
 * ========================================================================= */

/**
 * @brief Point d'entrée du test.
 *
 * Initialise l'interface, exécute une boucle d'affichage avec défilement
 * du curseur MIDI, puis détruit la fenêtre et retourne.
 *
 * @return 0 en cas de succès, 1 en cas d'erreur d'initialisation.
 */
int main(void)
{
    uiContext_t ctx;

    DBUTTON_setupButtons();

    printf("[test_ui] Initialisation de la fenêtre...\n");

    if (uiWindow_init(&ctx, PROJECT_NAME,
                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                      UI_WINDOW_DEFAULT_W, UI_WINDOW_DEFAULT_H) != 0) {
        fprintf(stderr, "[test_ui] Échec de l'initialisation de la fenêtre.\n");
        return 1;
    }

    printf("[test_ui] Fenêtre ouverte. Démarrage de la boucle de test (%d ms)...\n",
           TEST_DURATION_MS);

    int        cursorPos  = 0;
    Uint32     startTick  = SDL_GetTicks();
    int        running    = 1;
    SDL_Event  event;

    while (running) {
        /* Gestion des événements SDL. */
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_KEYDOWN &&
                event.key.keysym.sym == SDLK_ESCAPE) {
                running = 0;
            }
        }


        /* Avancement du curseur. */
        cursorPos = ((SDL_GetTicks() - startTick) / 5) % (UI_MIDI_TICK_MAX + 1);

#ifndef SIMULATED
        DBUTTON_scanButtons();
#endif
        DBUTTON_getButtonMap(s_buttonStates);

        /* Rendu. */
        uiRender_frame(&ctx,
                       s_buttonStates,
                       s_soundNames,
                       s_lineStates,
                       s_presses,
                       s_pressCounts,
                       cursorPos, 0);

        SDL_Delay(TEST_FRAME_MS);
    }

    printf("[test_ui] Fin du test. Fermeture de la fenêtre.\n");
    uiWindow_destroy(&ctx);
    return 0;
}
