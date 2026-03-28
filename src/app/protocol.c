/**
 *	\file		protocol.c
 *	\brief		Implémentation du protocole applicatif JoyPI
 *	\author		MARTEL Mathieu
 *	\version	1.0
 */
#include "app/protocol.h"

/*
*****************************************************************************************
 *	\noop		I M P L E M E N T A T I O N   DES   F O N C T I O N S
 */

short enum2status(statusRange_t type, action_t action) {
	return (type + 1) * 100 + action + 1;
}

statusRange_t getStatusRange(short code) {
	switch (code / 100) {
		case 1:		return REQ;
		case 2:		return ACK;
		case 3:		return ERR;
		default:	return ERR;
	}
}

action_t getAction(short code) {
	switch (code % 100) {
		case 1:		return CONNECT;
		case 2:		return BUTTON;
		case 3:		return SPECTRUM;
		case 4:		return PING;
		default:	return CONNECT;
	}
}