#ifndef FONTS_H
#define FONTS_H 120

#include "string.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
	uint8_t FontWidth;    // Comprimento da fonte em unidades de pixel
	uint8_t FontHeight;   // Altura da fonte em unidades de pixel
	const uint16_t *data; // Ponteiro para os dados
} FontDef_t;

typedef struct {
	uint16_t Length;      // Comprimento do caractere em unidades de pixel
	uint16_t Height;      // Altura do caractere em unidades de pixel
} FONTS_SIZE_t;

extern FontDef_t Font_7x10;

extern FontDef_t Font_11x18;

extern FontDef_t Font_16x26;

/* Função para obter as informações do caractere */
char* FONTS_GetStringSize(char* str, FONTS_SIZE_t* SizeStruct, FontDef_t* Font);
 
#endif
