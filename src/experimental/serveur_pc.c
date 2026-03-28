/**
 *	\file		serveur_pc.c
 *	\brief		Serveur PC : pipeline audio, calcul du spectre et diffusion au Joy-Pi
 *	\author		MARTEL Mathieu - ARCELON Louis
 *	\version	3.1
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

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <inet/session.h>

#include <drivers/buttons.h>

#include "audio/sdl_player.h"
#include "audio/fft_engine.h"
#include "audio/spectrum_mapper.h"

#include "app/dial.h"

/*
*****************************************************************************************
 *	\noop		C O N S T A N T E S
 */

/** @brief Adresse d'écoute (toutes les interfaces) */
#define BIND_ALL		"0.0.0.0"

/** @brief Port d'écoute du serveur PC */
#define PORT_SERVEUR	50001

/** @brief Fréquence d'affichage cible */
#define TARGET_FPS		25

/** @brief Durée d'une frame en ms */
#define FRAME_MS		(1000 / TARGET_FPS)

/*
*****************************************************************************************
 *	\noop		M A C R O S
 */

#define CHECK(sts, msg) if ((sts) == -1) { perror(msg); exit(-1); }

/*
*****************************************************************************************
 *	\noop		V A R I A B L E S   G L O B A L E S
 */

/** @brief Nom du programme */
char *progName;

/** @brief Données spectre partagées entre le pipeline audio et le thread réseau */
static SpectrumData_t	sharedSpectrum;

/** @brief Mutex de protection des données spectre partagées */
static pthread_mutex_t	mutexSpectrum = PTHREAD_MUTEX_INITIALIZER;

/** @brief Socket d'écoute du serveur */
static socket_t			sockEcoute;

/** @brief Sémaphore de fermeture propre */
static sem_t			semCanClose;

/**
 * @brief Sons pré-chargés pour chaque bouton
 *
 * Chargés depuis sounds/btn0.wav ... sounds/btn15.wav au démarrage.
 * Si un fichier est absent, la case correspondante reste NULL.
 */
static Mix_Chunk		*buttonSounds[MAX_BUTTONS];

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

static void initButtonSounds(char *sounds_dir) {
	char path[128];
	int i;

	for (i = 0; i < MAX_BUTTONS; i++) {
		sprintf(path, "%s/btn%d.wav", sounds_dir, i);
		buttonSounds[i] = Mix_LoadWAV(path);
		if (buttonSounds[i] == NULL)
			printf("[serveur] Son '%s' non trouve, bouton %d muet.\n", path, i);
	}
}

/*
*****************************************************************************************
 *	\noop		C A L L B A C K S
 */

static void onButtonUpdated(buttonStateMap_t map) {
	
	for (int i = 0; i < BUTTON_AMOUNT; i++) {
	
		if (map[i] != B_PRESSED) continue;
		Mix_PlayChannel(-1, buttonSounds[i], 0);
		printf("[serveur] Bouton %d -> son joue\n", i);
	}

}

/*
*****************************************************************************************
 *	\noop		F O N C T I O N S   R E S E A U
 */

static void *threadEcouteJoyPi(void *arg) {
	pcServDialParams_t	*params;
	socket_t			*sockDial;
	pthread_t			thread;

	printf("[serveur] En attente de connexion Joy-Pi sur le port %d...\n", PORT_SERVEUR);

	while (!mustDisconnect) {
		sockDial  = malloc(sizeof(socket_t));
		*sockDial = accepterClt(sockEcoute);

		printf("[serveur] Nouvelle connexion Joy-Pi acceptee.\n");

		params					= malloc(sizeof(pcServDialParams_t));
		params->sockDial		= sockDial;
		params->sharedSpectrum	= &sharedSpectrum;
		params->mutexSpectrum	= &mutexSpectrum;
		params->semCanClose		= &semCanClose;
		params->onButtonUpdated	= onButtonUpdated;

		pthread_create(&thread, NULL, (void *)(void *) dialSrvPC2Clt, params);
		pthread_detach(thread);
	}

	return NULL;
}

/*
*****************************************************************************************
 *	\noop		F O N C T I O N   P R I N C I P A L E
 */

void serveur() {
	float			pcm[PLAYER_FRAME_SIZE];
	float			magnitudes[FFT_BINS];
	BandFrame_t		bandFrame;
	pthread_t		threadReseau;
	Uint32			t0, elapsed;
	int				i;

	/* Initialisation du réseau */
	CHECK(sem_init(&semCanClose, 0, 0), "sem_init()");
	sockEcoute = creerSocketEcoute(BIND_ALL, PORT_SERVEUR);

	pthread_create(&threadReseau, NULL, threadEcouteJoyPi, NULL);
	pthread_detach(threadReseau);

	printf("[serveur] Pret. En attente de boutons (Ctrl+C pour quitter).\n");

	/* Boucle principale */
	/*
	 * La boucle tourne indefiniment, indépendamment de la lecture audio.
	 * Le pipeline FFT tourne en permanence : quand aucun son ne joue,
	 * sdl_player_read_frame() renvoie des echantillons silencieux et
	 * la matrice reste a zero.
	 */
	while (!mustDisconnect) {

		t0 = SDL_GetTicks();

		if (sdl_player_read_frame(pcm, PLAYER_FRAME_SIZE)) {

			fft_engine_compute(pcm, magnitudes);
			spectrum_mapper_compute(magnitudes, &bandFrame);

			pthread_mutex_lock(&mutexSpectrum);
			pthread_mutex_unlock(&mutexSpectrum);

		}

		elapsed = SDL_GetTicks() - t0;
		if (elapsed < FRAME_MS)
			SDL_Delay(FRAME_MS - elapsed);
	}

	/* Nettoyage */
	spectrum_mapper_reset();
	fft_engine_cleanup();
	sdl_player_cleanup();

	puts("[serveur] Arret.");
}

int main(int argc, char *argv[]) {
	progName = argv[0];

	(void)argc;

	fprintf(stderr, "Lancement du serveur PC [PID:%d] sur [%s:%d]\n",
		getpid(), BIND_ALL, PORT_SERVEUR);
		
	initSignaux();

	/* Initialisation des modules */
	if (sdl_player_init() != 0) {
		fprintf(stderr, "[serveur] Erreur initialisation audio\n");
		exit(EXIT_FAILURE);
	}

	if (fft_engine_init() != 0) {
		fprintf(stderr, "[serveur] Erreur initialisation FFT\n");
		sdl_player_cleanup();
		exit(EXIT_FAILURE);
	}

	if (spectrum_mapper_init() != 0) {
		fprintf(stderr, "[serveur] Erreur initialisation mapper\n");
		fft_engine_cleanup();
		sdl_player_cleanup();
		exit(EXIT_FAILURE);
	}
	
	if (argc > 1) {
		initButtonSounds(argv[1]);
	} else {
		initButtonSounds("sounds");
	}

	serveur();

	return EXIT_SUCCESS;
}