#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "Fonts.h"
#include "DisplaySSD1306.h"

#define SSD1306_I2C i2c1

#define ABSOLUTE(x) ((x) > 0 ? (x) : (-x))

/* Função para cálculo do quanto do buffer será destinado a renderização */
void Calculate_RenderArea_BufferLength(struct RenderArea *area){
    area->BufferLength = (area->EndColumn - area->StartColumn + 1) * (area->EndPage - area->StartPage + 1);
}

/* Função para enviar comandos ao display */
void SSD1306_SendCommand(uint8_t command){
    uint8_t dt[2] = {0x80, command};
    i2c_write_blocking(SSD1306_I2C, SSD1306_I2C_ADDRESS, dt, 2, false);
}

/* Função para enviar uma lista de comandos ao display */
void SSD1306_SendCommand_List(uint8_t *ssd, int number){
    for (int i = 0; i < number; i++){
        SSD1306_SendCommand(ssd[i]);
    }
}

/* Função para enviar dados ao display */
void SSD1306_SendBuffer(uint8_t ssd[], int buffer_length){
    uint8_t *aux_buffer = malloc(buffer_length + 1);

    aux_buffer[0] = 0x40;
    memcpy(aux_buffer + 1, ssd, buffer_length);

    i2c_write_blocking(SSD1306_I2C, SSD1306_I2C_ADDRESS, aux_buffer, buffer_length + 1, false);

    free(aux_buffer);
}

/* Função de inicialização do display */
void SSD1306_Init(){
    uint8_t Commands[] = {SSD1306_Set_Display, SSD1306_Set_MemoryMode, 0x00, SSD1306_Set_Display_StartLine, SSD1306_Set_SegmentRemap | 0x01,
                          SSD1306_Set_MuxRatio, SSD1306_HEIGHT - 1, SSD1306_Set_CommonOutputDirection | 0x08, SSD1306_Set_Offset, 0x00,
                          SSD1306_Set_CommonPinConfiguration, 0x12, SSD1306_Set_ClockDivideRatio, 0x80, SSD1306_Set_Precharge, 0xF1,
                          SSD1306_Set_VCOMH_DeselectLevel, 0x30, SSD1306_Set_Contrast, 0xFF, SSD1306_Set_EntireOn, SSD1306_Set_NormalDisplay,
                          SSD1306_Set_ChargePump, 0x14, SSD1306_DEACTIVATE_SCROLL | 0x00, SSD1306_Set_Display | 0x01};

    SSD1306_SendCommand_List(Commands, count_of(Commands));
}

/* Função para a renderização do display */
void SSD1306_Render(uint8_t *ssd, struct RenderArea *area){
    uint8_t Commands[] = {SSD1306_Set_ColumnAddress, area->StartColumn, area->EndColumn,
                          SSD1306_Set_PageAddress, area->StartPage, area->EndPage};

    SSD1306_SendCommand_List(Commands, count_of(Commands));
    SSD1306_SendBuffer(ssd, area->BufferLength);
}

/* Função para controlar um pixel desejado */
void SSD1306_SetPixel(uint8_t *ssd, int X, int Y, SSD1306_COLOR_t color) {
    assert(X >= 0 && X < SSD1306_WIDTH && Y >= 0 && Y < SSD1306_HEIGHT);

    const int bytes_per_row = SSD1306_WIDTH;

    int byte_idx = (Y / 8) * bytes_per_row + X;
    uint8_t byte = ssd[byte_idx];

    if (color) {
        byte |= 1 << (Y % 8);
    }
    else {
        byte &= ~(1 << (Y % 8));
    }

    ssd[byte_idx] = byte;
}

/* Função que desenha uma linha, seguindo o algoritmo de Bresenham */ 
void SSD1306_DrawLine(uint8_t *ssd, int X0, int Y0, int X1, int Y1, SSD1306_COLOR_t color){
    int16_t dx, dy, sx, sy, err, e2, i, tmp;

    if (X0 >= SSD1306_WIDTH){
        X0 = SSD1306_WIDTH - 1;
    }
    if (X1 >= SSD1306_WIDTH){
        X1 = SSD1306_WIDTH - 1;
    }
    if (Y0 >= SSD1306_HEIGHT){
        Y0 = SSD1306_HEIGHT - 1;
    }
    if (Y1 >= SSD1306_HEIGHT){
        Y1 = SSD1306_HEIGHT - 1;
    }

    dx = (X0 < X1) ? (X1 - X0) : (X0 - X1);
    dy = (Y0 < Y1) ? (Y1 - Y0) : (Y0 - Y1);
    sx = (X0 < X1) ? 1 : -1;
    sy = (Y0 < Y1) ? 1 : -1;
    err = ((dx > dy) ? dx : -dy) / 2;

    if (dx == 0){
        if (Y1 < Y0){
            tmp = Y1;
            Y1 = Y0;
            Y0 = tmp;
        }

        if (X1 < X0){
            tmp = X1;
            X1 = X0;
            X0 = tmp;
        }

        for (i = Y0; i <= Y1; i++){
            SSD1306_SetPixel(ssd, X0, i, color);
        }
        
        return;
    }

    if (dy == 0) {
		if (Y1 < Y0) {
			tmp = Y1;
			Y1 = Y0;
			Y0 = tmp;
		}
		
		if (X1 < X0) {
			tmp = X1;
			X1 = X0;
			X0 = tmp;
		}
		
		// Linha horizontal
		for (i = X0; i <= X1; i++) {
			SSD1306_SetPixel(ssd, i, Y0, color);
		}
		
		return;
	}

    while (1){
        SSD1306_SetPixel(ssd, X0, Y0, color);
        
        if (X0 == X1 && Y0 == Y1){
            break;
        }

        e2 = err;

        if (e2 > -dx){
            err -= dy;
            X0 += sx;
        }

        if (e2 < dy){
            err += dx;
            Y0 += sy;
        }
    }
}

/* Função para desenhar um retângulo */
void SSD1306_DrawRectangle(uint8_t *ssd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t color){
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT){
        return;
    }

    if ((x + w) >= SSD1306_WIDTH){
        w = SSD1306_WIDTH - x;
    }
    
    if ((y + h) >= SSD1306_HEIGHT){
        h = SSD1306_HEIGHT - y;
    }

    SSD1306_DrawLine(ssd, x, y, x + w, y, color);
    SSD1306_DrawLine(ssd, x, y + h, x + w, y + h, color);
    SSD1306_DrawLine(ssd, x, y, x, y + h, color);
    SSD1306_DrawLine(ssd, x + w, y, x + w, y + h, color);
}

/* Função para desenhar um retângulo preenchido */
void SSD1306_DrawFilledRectangle(uint8_t *ssd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t color){
    uint8_t i;

    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT){
        return;
    }

    if ((x + w) >= SSD1306_WIDTH){
        w = SSD1306_WIDTH - x;
    }

    if ((x + h) >= SSD1306_HEIGHT){
        h = SSD1306_HEIGHT - y;
    }

    for (i = 0; i <= h; i++){
        SSD1306_DrawLine(ssd, x, y + i, x + w, y + i, color);
    }
}

/* Função para desenhar um triângulo */
void SSD1306_DrawTriangle(uint8_t *ssd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color){
    SSD1306_DrawLine(ssd, x1, y1, x2, y2, color);
    SSD1306_DrawLine(ssd, x3, y3, x2, y2, color);
    SSD1306_DrawLine(ssd, x3, y3, x1, y1, color);
}

/* Função para desenhar um triângulo preenchido */
void SSD1306_DrawFilledTriangle(uint8_t *ssd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color) {
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
	yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
	curpixel = 0;
	
	deltax = ABSOLUTE(x2 - x1);
	deltay = ABSOLUTE(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		SSD1306_DrawLine(ssd, x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}

/* Função para desenhar um círculo */
void SSD1306_DrawCircle(uint8_t *ssd, int16_t x_center, int16_t y_center, int16_t radius, SSD1306_COLOR_t color){
    int16_t f = 1 - radius;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * radius;
    int16_t x = 0;
    int16_t y = radius;

    SSD1306_SetPixel(ssd, x_center, y_center + radius, color);
    SSD1306_SetPixel(ssd, x_center, y_center - radius, color);
    SSD1306_SetPixel(ssd, x_center + radius, y_center, color);
    SSD1306_SetPixel(ssd, x_center - radius, y_center, color);
    
    while (x < y){
        if (f >= 0){
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x;

        SSD1306_SetPixel(ssd, x_center + x, y_center + y, color);
        SSD1306_SetPixel(ssd, x_center - x, y_center + y, color);
        SSD1306_SetPixel(ssd, x_center + x, y_center - y, color);
        SSD1306_SetPixel(ssd, x_center - x, y_center - y, color);

        SSD1306_SetPixel(ssd, x_center + y, y_center + x, color);
        SSD1306_SetPixel(ssd, x_center - y, y_center + x, color);
        SSD1306_SetPixel(ssd, x_center + y, y_center - x, color);
        SSD1306_SetPixel(ssd, x_center - y, y_center - x, color);
    }
}

/* Função para desenhar um círculo preenchido */
void SSD1306_DrawFilledCircle(int8_t *ssd, int16_t x_center, int16_t y_center, int16_t radius, SSD1306_COLOR_t color){
    int16_t f = 1 - radius;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * radius;
    int16_t x = 0;
    int16_t y = radius;

    SSD1306_SetPixel(ssd, x_center, y_center + radius, color);
    SSD1306_SetPixel(ssd, x_center, y_center - radius, color);
    SSD1306_SetPixel(ssd, x_center + radius, y_center, color);
    SSD1306_SetPixel(ssd, x_center - radius, y_center, color);
    SSD1306_DrawLine(ssd, x_center - radius, y_center, x_center + radius, y_center, color);

    while (x < y){
        if (f >= 0){
            y--;
            ddF_y += 2;
            f += ddF_y;
        }

        x++;
        ddF_x += 2;
        f += ddF_x;

        SSD1306_DrawLine(ssd, x_center - x, y_center + y, x_center + x, y_center + y, color);
        SSD1306_DrawLine(ssd, x_center + x, y_center - y, x_center - x, y_center - y, color);
        
        SSD1306_DrawLine(ssd, x_center + y, y_center + x, x_center - y, y_center + x, color);
        SSD1306_DrawLine(ssd, x_center + y, y_center - x, x_center - y, y_center - x, color);
    }
}

/* Função para desenhar um arco */
void SSD1306_DrawArc(uint8_t *ssd, uint16_t x_center, uint16_t y_center, uint16_t radius, uint16_t start_angle, uint16_t end_angle, SSD1306_COLOR_t color){
    // Garante que os ângulos estão no intervalo correto
    if (start_angle > end_angle) {
        uint16_t temp = start_angle;
        start_angle = end_angle;
        end_angle = temp;
    }
    
    // Converte ângulos para radianos
    float start_rad = start_angle * M_PI / 180.0;
    float end_rad = end_angle * M_PI / 180.0;
    
    for (float angle = start_rad; angle <= end_rad; angle += M_PI / 90) {  // Incremento ajustado
        int x = x_center + (int)round(radius * cos(angle));
        int y = y_center - (int)round(radius * sin(angle));
        
        // Verifica se os pontos estão dentro dos limites do display (assumindo 128x64)
        if (x >= 0 && x < 128 && y >= 0 && y < 64) {
            SSD1306_SetPixel(ssd, x, y, color);
        }
    }
}

/* Função para desenhar um caractere */
void SSD1306_SetChar(uint8_t *ssd, int16_t x, int16_t y, uint8_t character, FontDef_t* Font, SSD1306_COLOR_t color) {
    uint32_t i, b, j;
    if (x > SSD1306_WIDTH - Font->FontWidth || y > SSD1306_HEIGHT - Font->FontHeight) {
        return; // Evita escrever fora dos limites da tela
    }

    character -= 32; // Ajusta o índice do caractere

	// Percorre o array dos caracteres
	for (i = 0; i < Font->FontHeight; i++) {
		b = Font->data[(character) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				SSD1306_SetPixel(ssd, x + j, y + i, color);
			} else {
				SSD1306_SetPixel(ssd, x + j, y + i, !color);
			}
		}
	}
}

/* Função para desenhar uma string */
void SSD1306_SetString(uint8_t *ssd, int16_t x, int16_t y, const char *str, FontDef_t* Font, SSD1306_COLOR_t color) {
    while (*str) {
        // Evita imprimir fora da tela na largura
        if (x + Font->FontWidth > SSD1306_WIDTH) {  
            x = 0;  
            y += Font->FontHeight;
        }

        // Evita imprimir fora da tela na altura
        if (y + Font->FontHeight > SSD1306_HEIGHT) {  
            return;
        }

        // Desenha o caractere atual
        SSD1306_SetChar(ssd, x, y, *str, Font, color);

        // Avança na horizontal
        x += Font->FontWidth + 1; // Adiciona um espaço entre caracteres

        // Avança para o próximo caractere na string
        str++;
    }
}

/* Função para desenhar uma string com quebra de linha */
void SSD1306_SetStringWrapped(uint8_t *ssd, int16_t x, int16_t y, const char *str, FontDef_t* Font, SSD1306_COLOR_t color) {
    while (*str) {
        if (*str == '\n') {  
            x = 0;  // Volta para o início da linha
            y += Font->FontHeight;  // Avança para a próxima linha
        } else {
            if (x + Font->FontWidth > SSD1306_WIDTH) {  
                x = 0;  
                y += Font->FontHeight;
            }

            if (y + Font->FontHeight > SSD1306_HEIGHT) {  
                return;  // Sai se ultrapassar a altura da tela
            }

            SSD1306_SetChar(ssd, x, y, *str, Font, color);
            x += Font->FontWidth; // Avança para o próximo caractere
        }

        str++; // Avança para o próximo caractere da string
    }
}

/* Função para limpar o display */
void SSD1306_Clear(uint8_t *ssd, struct RenderArea *area){
    memset(ssd, 0, SSD1306_BufferLength);
    SSD1306_Render(ssd, area);
}

void SSD1306_Invert(bool invert){
    if(invert){
        SSD1306_SendCommand(SSD1306_INVERTDISPLAY);
    }
    else{
        SSD1306_SendCommand(SSD1306_NORMALDISPLAY);
    }
}

void SSD1306_ON(){
    uint8_t Commands[] = {0x8D, 0x14, 0xAF};

    SSD1306_SendCommand_List(Commands, sizeof(Commands));
}

void SSD1306_OFF(){
    uint8_t Commands[] = {0x8D, 0x10, 0xAE};

    SSD1306_SendCommand_List(Commands, sizeof(Commands));
}