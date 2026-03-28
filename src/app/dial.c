/**
 *	\file		dial.c
 *	\brief		Implémentation des dialogues applicatifs PC <-> Joy-Pi
 *	\author		MARTEL Mathieu
 *	\version	1.0
 */

/*
*****************************************************************************************
 *	\noop		I N C L U D E S
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include <drivers/buttons.h>

#include "app/dial.h"

/*
*****************************************************************************************
 *	\noop		C O N S T A N T E S
 */

/* Codes de statut du protocole JoyPI */
#define REQ_CONNECT		101
#define ACK_CONNECT		201
#define ERR_CONNECT		301

#define REQ_BUTTON		102
#define ACK_BUTTON		202
#define ERR_BUTTON		302

#define ACK_SPECTRUM	203

#define REQ_PING		104
#define ACK_PING		204

/*
*****************************************************************************************
 *	\noop		V A R I A B L E S   G L O B A L E S
 */

volatile sig_atomic_t mustDisconnect = 0;

/*
*****************************************************************************************
 *	\noop		S T R U C T U R E S   I N T E R N E S
 */

/* Paramètres du thread interne d'envoi du spectre (côté serveur PC) */
typedef struct {
	socket_t		*sockDial;
	SpectrumData_t	*sharedSpectrum;
	pthread_mutex_t	*mutexSpectrum;
	volatile int	*running;
} spectrumSendParams_t;

/*
*****************************************************************************************
 *	\noop		F O N C T I O N S   I N T E R N E S
 */

static void *spectrumSendThread(void *arg) {
	spectrumSendParams_t	*params			= (spectrumSendParams_t *)arg;
	SpectrumData_t			local_spectrum;
	int						status;

	while (*params->running) {

		/* Copie locale sous mutex pour ne pas bloquer longtemps le pipeline audio */
		pthread_mutex_lock(params->mutexSpectrum);
		local_spectrum = *params->sharedSpectrum;
		pthread_mutex_unlock(params->mutexSpectrum);

		/* Envoi des 8 colonnes de la matrice au Joy-Pi */
		status = enum2status(ACK, SPECTRUM);
		sendResponse(params->sockDial, status, &local_spectrum, (pFct) spectrumData2str);

		SDL_Delay(SPECTRUM_INTERVAL_MS);
	}

	free(params);
	return NULL;
}

/*
*****************************************************************************************
 *	\noop		I M P L E M E N T A T I O N   DES   F O N C T I O N S
 */

void dialSrvPC2Clt(pcServDialParams_t *params) {

	req_t				request;
	buttonStateMap_t	buttonMap;
	spectrumSendParams_t *sendParams;
	pthread_t			sendThread;
	volatile int		running;
	int					status;

	socket_t		*sockDial		= params->sockDial;
	SpectrumData_t	*sharedSpectrum	= params->sharedSpectrum;
	pthread_mutex_t	*mutexSpectrum	= params->mutexSpectrum;
	sem_t			*semCanClose	= params->semCanClose;
	void (*onButtonUpdated)(buttonStateMap_t) = params->onButtonUpdated;

	free(params);

	/* Attente de la connexion du Joy-Pi */
	rcvRequest(sockDial, &request);

	if (request.id != REQ_CONNECT || request.verb != POST) {
		status = enum2status(ERR, CONNECT);
		sendResponse(sockDial, status, "Requete de connexion attendue", NULL);
		close(sockDial->fd);
		free(sockDial);
		return;
	}

	status = enum2status(ACK, CONNECT);
	sendResponse(sockDial, status, "Connexion OK", NULL);
	printf("[dial] Joy-Pi connecte.\n");

	/* Lancement du thread d'envoi périodique du spectre */
	running = 1;

	sendParams					= malloc(sizeof(spectrumSendParams_t));
	sendParams->sockDial		= sockDial;
	sendParams->sharedSpectrum	= sharedSpectrum;
	sendParams->mutexSpectrum	= mutexSpectrum;
	sendParams->running			= &running;

	pthread_create(&sendThread, NULL, spectrumSendThread, sendParams);
	pthread_detach(sendThread);

	/* Boucle de réception des requêtes boutons */
	while (1) {

		if (mustDisconnect) {
			running = 0;
			break;
		}

		rcvRequest(sockDial, &request);

		switch (request.id) {

			case REQ_BUTTON:

				if (request.verb == POST) {
					str2buttonMap(request.data, buttonMap);
					if (onButtonUpdated != NULL)
						onButtonUpdated(buttonMap);
					status = enum2status(ACK, BUTTON);
					sendResponse(sockDial, status, "OK", NULL);
				}
				break;

			case REQ_CONNECT:

				if (request.verb == DELETE) {
					running = 0;
					status = enum2status(ACK, CONNECT);
					sendResponse(sockDial, status, "Deconnexion OK", NULL);
					printf("[dial] Joy-Pi deconnecte.\n");
					sem_post(semCanClose);
					close(sockDial->fd);
					free(sockDial);
					return;
				}
				break;

			case REQ_PING:

				status = enum2status(ACK, PING);
				sendResponse(sockDial, status, "PONG", NULL);
				break;

			default:

				status = enum2status(ERR, getAction(request.id));
				sendResponse(sockDial, status, "Requete non geree", NULL);
				break;
		}
	}

	close(sockDial->fd);
	free(sockDial);
}

void dialClt2SrvPC(joypiCltDialParams_t *params) {

	rep_t			response;
	SpectrumData_t	spectrum;
	int				status;

	socket_t	*sockAppel				= params->sockAppel;
	sem_t		*semCanClose			= params->semCanClose;
	void (*onSpectrumReceived)(SpectrumData_t *) = params->onSpectrumReceived;

	free(params);

	/* Connexion au serveur PC */
	status = enum2status(REQ, CONNECT);
	sendRequest(sockAppel, status, POST, "JoyPi", NULL);

	rcvResponse(sockAppel, &response);

	if (response.id != ACK_CONNECT) {
		printf("[dial] Echec de connexion au PC (code %d).\n", response.id);
		sem_post(semCanClose);
		return;
	}

	printf("[dial] Connecte au serveur PC.\n");

	/* Boucle de réception du spectre */
	while (1) {

		if (mustDisconnect) {
			status = enum2status(REQ, CONNECT);
			sendRequest(sockAppel, status, DELETE, "", NULL);
			sem_post(semCanClose);
			return;
		}

		rcvResponse(sockAppel, &response);

		switch (response.id) {

			case ACK_SPECTRUM:

				str2spectrumData(response.data, &spectrum);
				if (onSpectrumReceived != NULL)
					onSpectrumReceived(&spectrum);
				break;

			case ACK_BUTTON:
				/* Rien à faire */
				break;

			case ACK_PING:
				/* Rien à faire */
				break;

			default:
				printf("[dial] Code reponse non gere : %d\n", response.id);
				break;
		}
	}
}

void envoyerBouton(socket_t *sockAppel, buttonStateMap_t map) {

	int status = enum2status(REQ, BUTTON);

	sendRequest(sockAppel, status, POST, map, (pFct) buttonMap2str);

}