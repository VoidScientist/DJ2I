/**
 *	\file		datastructs.c
 *	\brief		Sérialisation des structures de données JoyPI
 *	\author		MARTEL Mathieu
 *	\version	1.0
 */

/*
*****************************************************************************************
 *	\noop		I N C L U D E S
 */
#include <stdio.h>
#include "app/datastructs.h"

/*
*****************************************************************************************
 *	\noop		C O N S T A N T E S
 */

#define SPECTRUM_STR_OUT	"%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X"
#define SPECTRUM_STR_IN		"%X,%X,%X,%X,%X,%X,%X,%X"

/*
*****************************************************************************************
 *	\noop		I M P L E M E N T A T I O N   DES   F O N C T I O N S
 */

void buttonMap2str(buttonStateMap_t map, char *str) {
    int offset = 0;

    for (int i = 0; i < BUTTON_AMOUNT; i++) {
        offset += sprintf(str + offset, "%d,", map[i]);
    }

    if (offset > 0) {
        str[offset - 1] = '\0';
    }
}

void str2buttonMap(char *str, buttonStateMap_t map) {
	int value;

    for (int i = 0; i < BUTTON_AMOUNT; i++) {
        
        if (sscanf(str, "%d", &value) != 1) break;

        map[i] = (buttonState_t)value;

        while (*str && *str != ',') str++;
        if (*str == ',') str++;
    }

}

void spectrumData2str(SpectrumData_t *data, char *str) {
	sprintf(str, SPECTRUM_STR_OUT,
		data->columns[0], data->columns[1],
		data->columns[2], data->columns[3],
		data->columns[4], data->columns[5],
		data->columns[6], data->columns[7]);
}

void str2spectrumData(char *str, SpectrumData_t *data) {
	unsigned int vals[NB_COLUMNS];
	int i;

	sscanf(str, SPECTRUM_STR_IN,
		&vals[0], &vals[1], &vals[2], &vals[3],
		&vals[4], &vals[5], &vals[6], &vals[7]);

	for (i = 0; i < NB_COLUMNS; i++)
		data->columns[i] = (uint8_t)vals[i];
}