/**
 * @file drivers_common.h
 * @author ARCELON Louis
 * @brief Fichier en-têtes contenant les types partagés entre drivers.
 * @version 0.1
 * @date 2026-03-27
 */

#ifndef DRIVERS_COMMON_H
#define DRIVERS_COMMON_H

typedef enum {DRIVERS_OFF, DRIVERS_ON} powerState_t;

void DCOMMON_initWPi();

#endif /** DRIVERS_COMMON_H **/