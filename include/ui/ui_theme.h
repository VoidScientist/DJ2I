/**
 * @file ui_theme.h
 * @brief Macros de thème de l'interface : couleurs et dimensions.
 *
 * Toutes les constantes visuelles sont centralisées ici pour permettre
 * une personnalisation rapide en modifiant uniquement ce fichier.
 *
 * @version 0.1
 * @date 2026-03-22
 */

#ifndef UI_THEME_H
#define UI_THEME_H

/* =========================================================================
 * Couleurs – fond général
 * ========================================================================= */

/** @brief Composante rouge du fond de la fenêtre. */
#define UI_COLOR_BG_R        18
/** @brief Composante verte du fond de la fenêtre. */
#define UI_COLOR_BG_G        18
/** @brief Composante bleue du fond de la fenêtre. */
#define UI_COLOR_BG_B        22
/** @brief Composante alpha du fond de la fenêtre. */
#define UI_COLOR_BG_A       255

/* =========================================================================
 * Couleurs – titre et sous-titre
 * ========================================================================= */

/** @brief Composante rouge du titre principal. */
#define UI_COLOR_TITLE_R    230
/** @brief Composante verte du titre principal. */
#define UI_COLOR_TITLE_G    230
/** @brief Composante bleue du titre principal. */
#define UI_COLOR_TITLE_B    255
/** @brief Composante alpha du titre principal. */
#define UI_COLOR_TITLE_A    255

/** @brief Composante rouge du sous-titre (version). */
#define UI_COLOR_SUBTITLE_R 130
/** @brief Composante verte du sous-titre (version). */
#define UI_COLOR_SUBTITLE_G 130
/** @brief Composante bleue du sous-titre (version). */
#define UI_COLOR_SUBTITLE_B 170
/** @brief Composante alpha du sous-titre (version). */
#define UI_COLOR_SUBTITLE_A 255

/* =========================================================================
 * Couleurs – boutons de la grille
 * ========================================================================= */

/** @brief Composante rouge d'un bouton au repos. */
#define UI_COLOR_BTN_R            50
/** @brief Composante verte d'un bouton au repos. */
#define UI_COLOR_BTN_G            50
/** @brief Composante bleue d'un bouton au repos. */
#define UI_COLOR_BTN_B            65
/** @brief Composante alpha d'un bouton au repos. */
#define UI_COLOR_BTN_A           255

/** @brief Composante rouge d'un bouton pressé (assombri). */
#define UI_COLOR_BTN_PRESSED_R    20
/** @brief Composante verte d'un bouton pressé (assombri). */
#define UI_COLOR_BTN_PRESSED_G    20
/** @brief Composante bleue d'un bouton pressé (assombri). */
#define UI_COLOR_BTN_PRESSED_B    28
/** @brief Composante alpha d'un bouton pressé (assombri). */
#define UI_COLOR_BTN_PRESSED_A   255

/** @brief Composante rouge de la bordure d'un bouton. */
#define UI_COLOR_BTN_BORDER_R     90
/** @brief Composante verte de la bordure d'un bouton. */
#define UI_COLOR_BTN_BORDER_G     90
/** @brief Composante bleue de la bordure d'un bouton. */
#define UI_COLOR_BTN_BORDER_B    120
/** @brief Composante alpha de la bordure d'un bouton. */
#define UI_COLOR_BTN_BORDER_A    255

/** @brief Composante rouge du texte dans un bouton. */
#define UI_COLOR_BTN_TEXT_R      200
/** @brief Composante verte du texte dans un bouton. */
#define UI_COLOR_BTN_TEXT_G      200
/** @brief Composante bleue du texte dans un bouton. */
#define UI_COLOR_BTN_TEXT_B      220
/** @brief Composante alpha du texte dans un bouton. */
#define UI_COLOR_BTN_TEXT_A      255

/* =========================================================================
 * Couleurs – éditeur MIDI
 * ========================================================================= */

/** @brief Composante rouge d'une ligne MIDI activée. */
#define UI_COLOR_MIDI_LINE_R       80
/** @brief Composante verte d'une ligne MIDI activée. */
#define UI_COLOR_MIDI_LINE_G       80
/** @brief Composante bleue d'une ligne MIDI activée. */
#define UI_COLOR_MIDI_LINE_B      140
/** @brief Composante alpha d'une ligne MIDI activée. */
#define UI_COLOR_MIDI_LINE_A      255

/** @brief Composante rouge d'une ligne MIDI désactivée. */
#define UI_COLOR_MIDI_LINE_OFF_R   35
/** @brief Composante verte d'une ligne MIDI désactivée. */
#define UI_COLOR_MIDI_LINE_OFF_G   35
/** @brief Composante bleue d'une ligne MIDI désactivée. */
#define UI_COLOR_MIDI_LINE_OFF_B   50
/** @brief Composante alpha d'une ligne MIDI désactivée. */
#define UI_COLOR_MIDI_LINE_OFF_A  255

/** @brief Composante rouge d'un cercle MIDI (appui enregistré). */
#define UI_COLOR_MIDI_CIRCLE_R     80
/** @brief Composante verte d'un cercle MIDI (appui enregistré). */
#define UI_COLOR_MIDI_CIRCLE_G    200
/** @brief Composante bleue d'un cercle MIDI (appui enregistré). */
#define UI_COLOR_MIDI_CIRCLE_B    255
/** @brief Composante alpha d'un cercle MIDI (appui enregistré). */
#define UI_COLOR_MIDI_CIRCLE_A    255

/** @brief Composante rouge du texte d'un appui MIDI. */
#define UI_COLOR_MIDI_TEXT_R      160
/** @brief Composante verte du texte d'un appui MIDI. */
#define UI_COLOR_MIDI_TEXT_G      220
/** @brief Composante bleue du texte d'un appui MIDI. */
#define UI_COLOR_MIDI_TEXT_B      255
/** @brief Composante alpha du texte d'un appui MIDI. */
#define UI_COLOR_MIDI_TEXT_A      255

/** @brief Composante rouge de la ligne curseur (trotteuse). */
#define UI_COLOR_MIDI_CURSOR_R    255
/** @brief Composante verte de la ligne curseur (trotteuse). */
#define UI_COLOR_MIDI_CURSOR_G     80
/** @brief Composante bleue de la ligne curseur (trotteuse). */
#define UI_COLOR_MIDI_CURSOR_B     80
/** @brief Composante alpha de la ligne curseur (trotteuse). */
#define UI_COLOR_MIDI_CURSOR_A    200

/** @brief Composante rouge de la bordure de l'éditeur MIDI. */
#define UI_COLOR_MIDI_BORDER_R     60
/** @brief Composante verte de la bordure de l'éditeur MIDI. */
#define UI_COLOR_MIDI_BORDER_G     60
/** @brief Composante bleue de la bordure de l'éditeur MIDI. */
#define UI_COLOR_MIDI_BORDER_B     90
/** @brief Composante alpha de la bordure de l'éditeur MIDI. */
#define UI_COLOR_MIDI_BORDER_A    255

/* =========================================================================
 * Dimensions de la fenêtre
 * ========================================================================= */

/** @brief Largeur par défaut de la fenêtre en pixels. */
#define UI_WINDOW_DEFAULT_W  1028
/** @brief Hauteur par défaut de la fenêtre en pixels. */
#define UI_WINDOW_DEFAULT_H  600

/* =========================================================================
 * Dimensions de la grille 4x4
 * ========================================================================= */

/** @brief Nombre de colonnes dans la grille. */
#define UI_GRID_COLS          4
/** @brief Nombre de lignes dans la grille. */
#define UI_GRID_ROWS          4
/** @brief Taille d'un bouton (largeur et hauteur) en pixels. */
#define UI_GRID_BTN_SIZE     80
/** @brief Espacement entre les boutons en pixels. */
#define UI_GRID_PADDING      10
/** @brief Épaisseur du bord d'un bouton en pixels. */
#define UI_GRID_BORDER_W      2

/* =========================================================================
 * Dimensions de l'éditeur MIDI
 * ========================================================================= */

/** @brief Nombre de lignes verticales dans l'éditeur MIDI. */
#define UI_MIDI_TRACK_COUNT   4
/** @brief Rayon des cercles représentant les appuis enregistrés, en pixels. */
#define UI_MIDI_CIRCLE_RADIUS 7
/** @brief Valeur maximale de l'axe temporel (ticks). */
#define UI_MIDI_TICK_MAX   1000
/** @brief Épaisseur de la ligne curseur en pixels. */
#define UI_MIDI_CURSOR_W      2

/* =========================================================================
 * Dimensions du texte (police bitmap 8x8)
 * ========================================================================= */

/** @brief Largeur d'un caractère bitmap en pixels. */
#define UI_FONT_CHAR_W        8
/** @brief Hauteur d'un caractère bitmap en pixels. */
#define UI_FONT_CHAR_H        8
/** @brief Facteur d'échelle appliqué à la police lors du rendu. */
#define UI_FONT_SCALE_TITLE   3
/** @brief Facteur d'échelle pour le sous-titre. */
#define UI_FONT_SCALE_SUB     2
/** @brief Facteur d'échelle pour les textes des composants. */
#define UI_FONT_SCALE_UI      1

#endif /* UI_THEME_H */
