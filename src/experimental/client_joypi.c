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
#include <audio/timing.h>
#include <audio/record.h>

#include <drivers/buttons.h>
#include <drivers/led_matrix.h>
#include <drivers/segment.h>

#include "app/dial.h"

#include <ui/ui.h>

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
static recordNamedChunk_t	buttonSounds[MAX_BUTTONS];

static char        buttonNames[MAX_BUTTONS][MAX_RECORD_NAME_LENGTH];
static const char *buttonNamePtrs[MAX_BUTTONS];  // ← tableau de pointeurs

/**
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
    recordNamedChunk_t tmp;

    for (i = 0; i < MAX_BUTTONS; i++) {
        sprintf(path, "%s/btn%d.wav", SOUNDS_DIR, i);
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
 *	\noop		C A L L B A C K S   M O D E   C O N N E C T E
 */

static void onSpectrumReceived(SpectrumData_t *spectrum) {

	for (int b = 0; b < NUM_BANDS; b++) {

		unsigned char command = (1 << spectrum->columns[b]) - 1;

		DMATRIX_setColumn(b * 2, command);
		DMATRIX_setColumn(b * 2 + 1, command);

	}

	DMATRIX_renderBuffer();

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


		if (DBUTTON_changedLastFrame()) {

			DBUTTON_getButtonMap(map);

			envoyerBouton(&sockPC, map);

		}

	}

	do {
		result = sem_wait(&semCanClose);
	} while (result == -1 && errno == EINTR);

	DMATRIX_clearMatrix();

	pthread_join(threadDial, NULL);
}

/*
*****************************************************************************************
 *	\noop		M O D E   L O C A L
 */


void onNewTick(int currentTick) {

	RECORD_playRecorded(currentTick);
	DSEGMENT_displayNumber(currentTick);

}

int initLocal(uiContext_t *ctx) {

	TIMING_init(8, 1000, onNewTick);

	printf("[client] Mode local\n");

	/* Initialisation du pipeline audio complet */
	if (sdl_player_init() != 0) {
		fprintf(stderr, "[client] Erreur initialisation audio\n");
		return -1;
	}

	uiWindow_init(ctx, PROJECT_NAME, 
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
				UI_WINDOW_DEFAULT_W, UI_WINDOW_DEFAULT_H);

	if (fft_engine_init() != 0) {
		fprintf(stderr, "[client] Erreur initialisation FFT\n");
		sdl_player_cleanup();
		return -1;
	}

	if (spectrum_mapper_init() != 0) {
		fprintf(stderr, "[client] Erreur initialisation mapper\n");
		fft_engine_cleanup();
		sdl_player_cleanup();
		return -1;
	}

	initButtonSounds();

}

static void lancerModeLocal() {
	float			pcm[PLAYER_FRAME_SIZE];
	float			magnitudes[FFT_BINS];
	BandFrame_t		bandFrame;
	buttonStateMap_t map;
	uiContext_t 	ctx;
	uiRecordedPress  records[MAX_RECORD_CHANNELS][MAX_RECORDINGS];
	uiRecordedPress *recordPtrs[MAX_RECORD_CHANNELS];
	int 			amountRecorded[MAX_RECORD_CHANNELS];
	SDL_Event		event;

	int channelStates[4];

	int start = 0;
	int timer = 0;

	int isPaused = 0;
	int isRecording = 1;
	int currentRecordChannel = 0;

	int delta = 0;


	if (initLocal(&ctx) < 0) return;

	for (int i = 0; i < MAX_RECORD_CHANNELS; i++) {
    	recordPtrs[i] = records[i];
	}

	/* Boucle principale */
	while (!mustDisconnect) {

		start = SDL_GetTicks();


		DBUTTON_scanButtons();


		if (DBUTTON_changedLastFrame()) {

			DBUTTON_getButtonMap(map);

			for (int i = 0; i < BUTTON_AMOUNT; i++) {

				if (map[i] != B_PRESSED) continue;
		
				int row = i / 4;
				int col = i % 4;

				if (row == 3) {
					switch (col) {

						case 0: {
							RECORD_clearRecordings();
							currentRecordChannel = 0;
							isPaused = 1;
							TIMING_setPause(isPaused);
							TIMING_reset();
							DSEGMENT_displayNumber(TIMING_getCurrentTick());
							RECORD_setCurrentChannel(currentRecordChannel);
							break;
						}

						case 1: {
							if (currentRecordChannel >= MAX_RECORD_CHANNELS) break;
							currentRecordChannel = (currentRecordChannel + 1) % MAX_RECORD_CHANNELS;
							RECORD_setCurrentChannel(currentRecordChannel);
							break;
						}
						case 2: {
							isRecording = !isRecording;
							break;
						}
						case 3: {
							isPaused = !isPaused;
							TIMING_setPause(isPaused);
							break;
						}

					}
				} else {
					sdl_player_play_chunk(buttonSounds[i].chunk);

					if (isRecording && !isPaused) {
						RECORD_recordPress(&buttonSounds[i], TIMING_getCurrentTick());
					}

				}


			}

		}

		if (sdl_player_read_frame(pcm, PLAYER_FRAME_SIZE)) {

            fft_engine_compute(pcm, magnitudes);
            spectrum_mapper_compute(magnitudes, &bandFrame);


            for (int b = 0; b < NUM_BANDS; b++) {

                unsigned char command = (1 << bandFrame.heights[b]) - 1;

                DMATRIX_setColumn(b * 2, command);
                DMATRIX_setColumn(b * 2 + 1, command);

            }

            DMATRIX_renderBuffer();

        }

		RECORD_getActiveChannelsArray(channelStates);
		RECORD_getRecordedPresses(recordPtrs, amountRecorded);

		uiRender_frame(&ctx, map, buttonNamePtrs, channelStates,
					(const uiRecordedPress *const *)recordPtrs,
					amountRecorded,
					TIMING_getCurrentTick(), isRecording);

		while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                mustDisconnect = 1;
				break;
            }
        }
		
		delta = SDL_GetTicks() - start;

		timer += delta;
		TIMING_update(delta);
		
	}

	DSEGMENT_setPowerState(DRIVERS_OFF);
	DMATRIX_setOperationMode(DRIVERS_OFF);

	/* Nettoyage */
	spectrum_mapper_reset();
	fft_engine_cleanup();
	sdl_player_cleanup();

	uiWindow_destroy(&ctx);

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
	DMATRIX_setupMatrix();
	DSEGMENT_setupSegment();


	if (mode == MODE_LOCAL) {
		lancerModeLocal();
	} else {
		lancerModeConnecte(ip, port);
	}

	puts("[client] Arret.");

	return EXIT_SUCCESS;
}