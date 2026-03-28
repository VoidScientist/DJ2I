/**
 *	\file		protocol.h
 *	\brief		Définitions du protocole applicatif JoyPI
 *	\author		MARTEL Mathieu
 *	\version	1.0
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

/*
*****************************************************************************************
 *	\noop		C O N S T A N T E S
 */

/** @brief Verbe GET : demande d'information */
#define GET		0
/** @brief Verbe POST : envoi d'information */
#define POST	1
/** @brief Verbe DELETE : déconnexion */
#define DELETE	2

/*
*****************************************************************************************
 *	\noop		E N U M E R A T I O N S
 */

/**
 * @brief Intervalles des codes de statut
 *
 * REQ : 100-199 (requêtes)
 * ACK : 200-299 (réponses réussies)
 * ERR : 300-399 (erreurs)
 */
typedef enum { REQ=0, ACK=1, ERR=2 } statusRange_t;

/**
 * @brief Actions du protocole applicatif
 *
 * CONNECT  : connexion ou déconnexion
 * BUTTON   : événement bouton Joy-Pi -> PC
 * SPECTRUM : données spectre PC -> Joy-Pi
 * PING     : vérification de la connexion
 */
typedef enum { CONNECT=0, BUTTON=1, SPECTRUM=2, PING=3 } action_t;

/*
*****************************************************************************************
 *	\noop		F O N C T I O N S
 */

/**
 * @brief Convertit un intervalle et une action en code de statut entier
 *
 * @param type		Intervalle du code (REQ, ACK, ERR)
 * @param action	Action correspondante
 *
 * @return Code de statut entier
 */
short enum2status(statusRange_t type, action_t action);

/**
 * @brief Récupère l'intervalle de statut d'un code
 *
 * @param code	Code de statut
 *
 * @return Intervalle correspondant (REQ, ACK ou ERR)
 */
statusRange_t getStatusRange(short code);

/**
 * @brief Récupère l'action correspondant à un code de statut
 *
 * @param code	Code de statut
 *
 * @return Action correspondante
 */
action_t getAction(short code);

#endif /* PROTOCOL_H */