/**
 *	\file		client_joypi.c
 *	\brief		Client Joy-Pi : mode connecté (PC) ou mode local (autonome)
 *	\author		MARTEL Mathieu, ARCELON Louis
 *	\version	2.0
 */

/*
*****************************************************************************************
 *	\noop		I N C L U D E S
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <sys/select.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <inet/session.h>

#include "audio/sdl_player.h"
#include "audio/fft_engine.h"
#include "audio/spectrum_mapper.h"

#include <drivers/buttons.h>

#include "app/dial.h"

/*
*****************************************************************************************
 *	\noop		C O N S T A N T E S
 */

/** @brief Adresse du serveur PC par défaut */
#define DEFAULT_PC_IP	"127.0.0.1"

/** @brief Port du serveur PC */
#define PORT_SERVEUR	50001

/** @brief Fréquence d'affichage cible */
#define TARGET_FPS		25

/** @brief Durée d'une frame en ms */
#define FRAME_MS		(1000 / TARGET_FPS)

/** @brief Dossier contenant les sons des boutons */
#define SOUNDS_DIR		"sounds"

#define NUM_BUTTONS		16

/*
*****************************************************************************************
 *	\noop		M A C R O S
 */

#define CHECK(sts, msg) if ((sts) == -1) { perror(msg); exit(-1); }

/*
*****************************************************************************************
 *	\noop		E N U M E R A T I O N S
 */

/** @brief Modes de fonctionnement du Joy-Pi */
typedef enum {
	MODE_CONNECTE,	/**< Son sur le PC, spectre reçu par réseau */
	MODE_LOCAL		/**< Son et spectre traités localement       */
} JoyPiMode_t;

/*
*****************************************************************************************
 *	\noop		V A R I A B L E S   G L O B A L E S
 */

/** @brief Nom du programme */
char *progName;

/** @brief Socket de connexion au serveur PC (mode connecté) */
static socket_t		sockPC;

/** @brief Sémaphore de fermeture propre */
static sem_t		semCanClose;

/** @brief Thread de dialogue avec le serveur PC (mode connecté) */
static pthread_t	threadDial;

/**
 * @brief Sons pré-chargés pour chaque bouton
 *
 * Chargés depuis sounds/btn0.wav ... sounds/btn15.wav au démarrage.
 * Utilisés en mode local et en mode connecté.
 */
static Mix_Chunk	*buttonSounds[MAX_BUTTONS];

/*
*****************************************************************************************
 *	\noop		F O N C T I O N S   D ' I N I T I A L I S A T I O N
 */

static void onSignal(int code) {
	mustDisconnect = (code == SIGINT);
}

static void initSignaux() {
	struct sigaction sa;
	CHECK(sigemptyset(&sa.sa_mask), "sigemptyset()");
	sa.sa_handler	= onSignal;
	sa.sa_flags		= 0;
	CHECK(sigaction(SIGINT, &sa, NULL), "sigaction()");
}

static void initButtonSounds() {
	char path[128];
	int i;

	for (i = 0; i < MAX_BUTTONS; i++) {
		sprintf(path, "%s/btn%d.wav", SOUNDS_DIR, i);
		buttonSounds[i] = Mix_LoadWAV(path);
		if (buttonSounds[i] == NULL)
			printf("[client] Son '%s' non trouve, bouton %d muet.\n", path, i);
	}
}

static void jouerSon(uint8_t button_id) {
	if (button_id >= MAX_BUTTONS)		return;
	if (buttonSounds[button_id] == NULL)	return;
	Mix_PlayChannel(-1, buttonSounds[button_id], 0);
}

/*
*****************************************************************************************
 *	\noop		C A L L B A C K S   M O D E   C O N N E C T E
 */

/*
 * Reconstitue la hauteur d'une barre à partir de son registre uint8_t.
 * Registre = (1 << (h+1)) - 1, donc hauteur = position du bit le plus haut.
 */
static uint8_t registreVersHauteur(uint8_t registre) {
	uint8_t hauteur = 0;
	uint8_t tmp     = registre >> 1;	/* bit 0 = ligne de base, on ignore */

	while (tmp) {
		hauteur++;
		tmp >>= 1;
	}

	return (hauteur > MAX_HEIGHT) ? MAX_HEIGHT : hauteur;
}

static void onSpectrumReceived(SpectrumData_t *spectrum) {

}

/*
*****************************************************************************************
 *	\noop		M O D E   C O N N E C T E
 */

static void lancerModeConnecte(char *ip, int port) {
	joypiCltDialParams_t	*params;
	int						result;
	buttonStateMap_t		map;

	CHECK(sem_init(&semCanClose, 0, 0), "sem_init()");

	printf("[client] Mode connecte | Connexion a %s:%d...\n", ip, port);
	sockPC = connecterClt2Srv(ip, (short)port);
	printf("[client] Connexion etablie.\n");

	params					= malloc(sizeof(joypiCltDialParams_t));
	params->sockAppel		= &sockPC;
	params->semCanClose		= &semCanClose;
	params->onSpectrumReceived	= onSpectrumReceived;

	pthread_create(&threadDial, NULL, (void *)(void *) dialClt2SrvPC, params);

	while (!mustDisconnect) {
		DBUTTON_scanButtons();

		for (int i = 0; i < NUM_BUTTONS; i++) {

			DBUTTON_getButtonMap(map, BUTTON_AMOUNT);

			envoyerBouton(&sockPC, map);

		}

	}

	do {
		result = sem_wait(&semCanClose);
	} while (result == -1 && errno == EINTR);

	pthread_join(threadDial, NULL);
}

/*
*****************************************************************************************
 *	\noop		M O D E   L O C A L
 */

static void lancerModeLocal() {
	float			pcm[PLAYER_FRAME_SIZE];
	float			magnitudes[FFT_BINS];
	BandFrame_t		bandFrame;
	Uint32			t0, elapsed;
	char			input;

	printf("[client] Mode local\n");

	/* Initialisation du pipeline audio complet */
	if (sdl_player_init() != 0) {
		fprintf(stderr, "[client] Erreur initialisation audio\n");
		return;
	}

	if (fft_engine_init() != 0) {
		fprintf(stderr, "[client] Erreur initialisation FFT\n");
		sdl_player_cleanup();
		return;
	}

	if (spectrum_mapper_init() != 0) {
		fprintf(stderr, "[client] Erreur initialisation mapper\n");
		fft_engine_cleanup();
		sdl_player_cleanup();
		return;
	}

	initButtonSounds();

	fputs("\033[2J\033[H\033[?25l", stdout);
	printf("Joy-Pi | Mode local | %d FPS\n", TARGET_FPS);
	printf("Touches 0-9 : jouer un son | q : quitter\n");
	fflush(stdout);

	/* Boucle principale */
	while (!mustDisconnect) {
		t0 = SDL_GetTicks();

		/* Lecture non bloquante d'un bouton (stdin ici, GPIO sur Joy-Pi réel) */
		fd_set fds;
		struct timeval tv = {0, 0};
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);

		if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
			input = (char)getchar();

			if (input == 'q' || input == 'Q') {
				mustDisconnect = 1;
				break;
			}

			if (input >= '0' && input <= '9') {
				jouerSon((uint8_t)(input - '0'));
			}
		}

		/* Pipeline FFT -> affichage matrice */
		if (sdl_player_read_frame(pcm, PLAYER_FRAME_SIZE)) {
			fft_engine_compute(pcm, magnitudes);
			spectrum_mapper_compute(magnitudes, &bandFrame);

		}

		elapsed = SDL_GetTicks() - t0;
		if (elapsed < FRAME_MS)
			SDL_Delay(FRAME_MS - elapsed);
	}

	/* Nettoyage */
	spectrum_mapper_reset();
	fft_engine_cleanup();
	sdl_player_cleanup();

	fputs("\033[?25h\n", stdout);
}

/*
*****************************************************************************************
 *	\noop		F O N C T I O N   P R I N C I P A L E
 */

int main(int argc, char *argv[]) {
	JoyPiMode_t mode;
	char		*ip;
	int			port;

	progName = argv[0];
	initSignaux();

	/*
	 * Usage :
	 *   ./client_joypi local              -> mode local
	 *   ./client_joypi 192.168.1.10 50001 -> mode connecté
	 */
	if (argc >= 2 && strcmp(argv[1], "local") == 0) {
		mode = MODE_LOCAL;
	} else if (argc >= 3) {
		mode	= MODE_CONNECTE;
		ip		= argv[1];
		port	= atoi(argv[2]);
	} else {
		fprintf(stderr, "usage :\n");
		fprintf(stderr, "  %s local              -> mode local\n", basename(progName));
		fprintf(stderr, "  %s @IP port           -> mode connecte\n", basename(progName));
		return EXIT_FAILURE;
	}

	fprintf(stderr, "Lancement du client Joy-Pi [PID:%d] en mode %s\n",
		getpid(), (mode == MODE_LOCAL) ? "LOCAL" : "CONNECTE");

	DBUTTON_setupButtons();

	if (mode == MODE_LOCAL) {
		lancerModeLocal();
	} else {
		lancerModeConnecte(ip, port);
	}

	puts("[client] Arret.");

	return EXIT_SUCCESS;
}
