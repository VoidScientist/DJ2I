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

#include <audio/timing.h>
#include <audio/record.h>

#include "app/dial.h"

#include <ui/ui.h>

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
static recordNamedChunk_t	buttonSounds[MAX_BUTTONS];

static char        buttonNames[MAX_BUTTONS][MAX_RECORD_NAME_LENGTH];
static const char *buttonNamePtrs[MAX_BUTTONS];  // ← tableau de pointeurs

static buttonStateMap_t buttonMap;

static int isPaused = 0;
static int isRecording = 1;
static int currentRecordChannel = 0;

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
    recordNamedChunk_t tmp;

    for (i = 0; i < MAX_BUTTONS; i++) {
        sprintf(path, "%s/btn%d.wav", sounds_dir, i);
        snprintf(tmp.name, MAX_RECORD_NAME_LENGTH, "btn%d", i);
        snprintf(buttonNames[i], MAX_RECORD_NAME_LENGTH, "btn%d", i);
        buttonNamePtrs[i] = buttonNames[i];  // pointe vers le string
        tmp.chunk = Mix_LoadWAV(path);
        if (tmp.chunk == NULL)
            printf("[client] Son '%s' non trouve, bouton %d muet.\n", path, i);
        buttonSounds[i] = tmp;
    }
}

/*
*****************************************************************************************
 *	\noop		C A L L B A C K S
 */

static void onButtonUpdated(buttonStateMap_t map) {
	
	for (int i = 0; i < BUTTON_AMOUNT; i++) {
	
		buttonMap[i] = map[i];
		if (map[i] != B_PRESSED) continue;
		sdl_player_play_chunk(buttonSounds[i].chunk);

		if (isRecording && !isPaused) {
			RECORD_recordPress(&buttonSounds[i], TIMING_getCurrentTick());
		}

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

void onNewTick(int currentTick) {

	printf("\rCurrent Tick: %04d", currentTick);
	fflush(stdout);
	RECORD_playRecorded(currentTick);

}

void serveur(uiContext_t *ctx) {
	float			pcm[PLAYER_FRAME_SIZE];
	float			magnitudes[FFT_BINS];
	BandFrame_t		bandFrame;
	pthread_t		threadReseau;
	Uint32			t0, elapsed;
	int				i;

	uiRecordedPress  records[MAX_RECORD_CHANNELS][MAX_RECORDINGS];
	uiRecordedPress *recordPtrs[MAX_RECORD_CHANNELS];
	int 			amountRecorded[MAX_RECORD_CHANNELS];
	SDL_Event		event;

	int channelStates[4];

	for (int i = 0; i < MAX_RECORD_CHANNELS; i++) {
    	recordPtrs[i] = records[i];
	}


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
			for (i = 0; i < NB_COLUMNS; i++)
				sharedSpectrum.columns[i] = bandFrame.heights[i];
			pthread_mutex_unlock(&mutexSpectrum);

		}

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				mustDisconnect = 1;
				break;
			}

			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {

					case SDLK_p: {
						isPaused = !isPaused;
						TIMING_setPause(isPaused);
						break;
					}

					case SDLK_r: {
						isRecording = !isRecording;
						break;
					}

					case SDLK_n: {
						if (currentRecordChannel >= MAX_RECORD_CHANNELS) break;
						currentRecordChannel = (currentRecordChannel + 1) % MAX_RECORD_CHANNELS;
						RECORD_setCurrentChannel(currentRecordChannel);
						break;
					}

					case SDLK_z: {
						RECORD_clearRecordings();
						currentRecordChannel = 0;
						isPaused = 1;
						TIMING_setPause(isPaused);
						TIMING_reset();
						RECORD_setCurrentChannel(currentRecordChannel);
						break;
					}

				}
			}
		}

		RECORD_getActiveChannelsArray(channelStates);
		RECORD_getRecordedPresses(recordPtrs, amountRecorded);

		uiRender_frame(ctx, buttonMap, buttonNamePtrs, channelStates,
					(const uiRecordedPress *const *)recordPtrs,
					amountRecorded,
					TIMING_getCurrentTick(), isRecording);


		elapsed = SDL_GetTicks() - t0;
		TIMING_update(FRAME_MS);
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
	uiContext_t ctx;

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

	uiWindow_init(&ctx, PROJECT_NAME, 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			UI_WINDOW_DEFAULT_W, UI_WINDOW_DEFAULT_H);

	TIMING_init(8, 1000, &onNewTick);
	
	if (argc > 1) {
		initButtonSounds(argv[1]);
	} else {
		initButtonSounds("sounds");
	}

	serveur(&ctx);

	return EXIT_SUCCESS;
}