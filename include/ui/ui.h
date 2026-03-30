/**
 * @file ui.h
 * @brief Point d'entrée unique de l'API publique de l'interface looper/séquenceur.
 *
 * L'intégrateur n'a besoin d'inclure que ce seul fichier pour accéder à
 * l'ensemble des symboles publics du module UI :
 *  - Types et structures (@ref uiContext_t, @ref uiRecordedPress)
 *  - Macros de thème (couleurs, dimensions) depuis @ref ui_theme.h
 *  - Gestion de fenêtre (@ref uiWindow_init, @ref uiWindow_destroy, …)
 *  - Rendu de texte bitmap (@ref uiFont_drawText, …)
 *  - Composant grille 4×4 (@ref uiGrid_draw)
 *  - Composant éditeur MIDI (@ref uiMidi_draw)
 *  - Rendu de frame complet (@ref uiRender_frame)
 *
 * @par Exemple d'utilisation minimal
 * @code
 * #include "ui/ui.h"
 *
 * int main(void) {
 *     uiContext_t ctx;
 *     uiWindow_init(&ctx, PROJECT_NAME,
 *                   SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
 *                   UI_WINDOW_DEFAULT_W, UI_WINDOW_DEFAULT_H);
 *
 *     // boucle principale ...
 *     uiRender_frame(&ctx, buttonStates, soundNames,
 *                    lineStates, presses, pressCounts, cursorPos);
 *
 *     uiWindow_destroy(&ctx);
 *     return 0;
 * }
 * @endcode
 *
 * @version 0.1
 * @date 2026-03-22
 */

#ifndef UI_H
#define UI_H

#include "ui/ui_theme.h"
#include "ui/ui_types.h"
#include "ui/ui_font.h"
#include "ui/ui_window.h"
#include "ui/ui_grid.h"
#include "ui/ui_midi.h"
#include "ui/ui_render.h"

#endif /* UI_H */
