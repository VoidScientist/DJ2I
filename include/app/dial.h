/**
 *	\file		dial.h
 *	\brief		Dialogues applicatifs entre le PC serveur et le Joy-Pi client
 *	\author		MARTEL Mathieu
 *	\version	1.0
 */

#ifndef DIAL_H
#define DIAL_H

/*
*****************************************************************************************
 *	\noop		I N C L U D E S
 */
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include <inet/session.h>
#include <app/repReq.h>

#include <drivers/buttons.h>

#include "app/protocol.h"
#include "app/datastructs.h"

/*
*****************************************************************************************
 *	\noop		C O N S T A N T E S
 */

/** @brief Intervalle d'envoi du spectre en millisecondes (~25 FPS) */
#define SPECTRUM_INTERVAL_MS	40

/*
*****************************************************************************************
 *	\noop		V A R I A B L E S   G L O B A L E S
 */

/** @brief Flag de demande de déconnexion (modifié par le gestionnaire de signal) */
extern volatile sig_atomic_t mustDisconnect;

/*
*****************************************************************************************
 *	\noop		S T R U C T U R E S
 */

/**
 * @brief Paramètres du thread de dialogue côté serveur PC
 *
 * Un thread dialSrvPC2Clt est lancé par connexion Joy-Pi acceptée.
 * Il gère la réception des boutons et lance en interne un thread d'envoi
 * du spectre toutes les SPECTRUM_INTERVAL_MS ms.
 */
typedef struct {
	socket_t		*sockDial;			/**< Socket de dialogue avec le Joy-Pi        */
	SpectrumData_t	*sharedSpectrum;	/**< Données spectre partagées avec le pipeline audio */
	pthread_mutex_t	*mutexSpectrum;		/**< Mutex de protection du spectre partagé   */
	sem_t			*semCanClose;		/**< Sémaphore de fermeture propre            */
	void (*onButtonUpdated)(buttonStateMap_t); /**< Callback bouton */
} pcServDialParams_t;

/**
 * @brief Paramètres du thread de dialogue côté client Joy-Pi
 *
 * Le thread dialClt2SrvPC gère la réception des données spectre et
 * appelle le callback d'affichage à chaque trame reçue.
 * L'envoi des boutons se fait depuis le thread principal via envoyerBouton().
 */
typedef struct {
	socket_t	*sockAppel;			/**< Socket de connexion au PC                */
	sem_t		*semCanClose;		/**< Sémaphore de fermeture propre            */
	void (*onSpectrumReceived)(SpectrumData_t *spectrum); /**< Callback spectre   */
} joypiCltDialParams_t;

/*
*****************************************************************************************
 *	\noop		F O N C T I O N S
 */

/**
 * @brief Thread de dialogue côté serveur PC pour un Joy-Pi connecté
 *
 * Séquence :
 *   1. Attend la requête CONNECT POST du Joy-Pi
 *   2. Répond ACK_CONNECT si accepté
 *   3. Lance un thread interne d'envoi du spectre (toutes les SPECTRUM_INTERVAL_MS ms)
 *   4. Boucle sur rcvRequest pour traiter les boutons et le ping
 *   5. Gère la déconnexion propre sur CONNECT DELETE ou mustDisconnect
 *
 * @param params	Paramètres du thread (pcServDialParams_t, alloué avec malloc)
 */
void dialSrvPC2Clt(pcServDialParams_t *params);

/**
 * @brief Thread de dialogue côté client Joy-Pi
 *
 * Séquence :
 *   1. Envoie CONNECT POST au serveur PC
 *   2. Attend ACK_CONNECT
 *   3. Boucle sur rcvResponse pour recevoir le spectre et les ACK boutons
 *   4. Appelle onSpectrumReceived à chaque trame de spectre reçue
 *   5. Gère la déconnexion sur mustDisconnect
 *
 * @param params	Paramètres du thread (joypiCltDialParams_t, alloué avec malloc)
 */
void dialClt2SrvPC(joypiCltDialParams_t *params);


void envoyerBouton(socket_t *sockAppel, buttonStateMap_t map);

#endif /* DIAL_H */