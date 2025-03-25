#ifndef SSD1306_H
#define SSD1306_H

#include "Fonts.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "hardware/i2c.h"

/* Endereço I2C do display */
#ifndef SSD1306_I2C_ADDRESS
#define SSD1306_I2C_ADDRESS 0x3C
//#define SSD1306_I2C_ADDRESS 0x7A
#endif

/* Comprimento do display */
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH 128
#endif

/* Altura do display */
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT 64
#endif

/* Endereços para configuração do display */
#define SSD1306_Set_MemoryMode 0x20
#define SSD1306_Set_ColumnAddress 0x21
#define SSD1306_Set_PageAddress 0x22
#define SSD1306_Set_Display_StartLine 0x40
#define SSD1306_Set_Contrast 0x81
#define SSD1306_Set_ChargePump 0x8D
#define SSD1306_Set_SegmentRemap 0xA0
#define SSD1306_Set_EntireOn 0xA4
#define SSD1306_Set_AllOn 0xA5
#define SSD1306_Set_NormalDisplay 0xA6
#define SSD1306_Set_InverserDisplay 0xA7
#define SSD1306_Set_MuxRatio 0xA8
#define SSD1306_Set_Display 0xAE
#define SSD1306_Set_CommonOutputDirection 0xC0
#define SSD1306_Set_CommonOutputDirection_Flip 0xC8
#define SSD1306_Set_Offset 0xD3
#define SSD1306_Set_ClockDivideRatio 0xD5
#define SSD1306_Set_Precharge 0xD9
#define SSD1306_Set_CommonPinConfiguration 0xDA
#define SSD1306_Set_VCOMH_DeselectLevel 0xD8

#define SSD1306_PageHeight 8
#define SSD1306_NumberOfPages (SSD1306_HEIGHT / SSD1306_PageHeight)
#define SSD1306_BufferLength (SSD1306_NumberOfPages * SSD1306_WIDTH)

#define SSD1306_WriteMode 0xFE
#define SSD1306_ReadMode 0xFF

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A
#define SSD1306_DEACTIVATE_SCROLL                    0x2E 
#define SSD1306_ACTIVATE_SCROLL                      0x2F 
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3 

#define SSD1306_NORMALDISPLAY       0xA6 
#define SSD1306_INVERTDISPLAY       0xA7 

/* - Estrutura utilizada para a área de renderização
   - Deve ser inicializada no início do código */
struct RenderArea {
   uint8_t StartColumn;
   uint8_t EndColumn;
   uint8_t StartPage;
   uint8_t EndPage;
   int BufferLength;
};

/* Estrutura para as características do display */
typedef struct {
   uint8_t Width, Height, Pages, Address;
   i2c_inst_t * i2c_port;
   bool external_vcc;
   uint8_t *ram_buffer;
   size_t bufsize;
   uint8_t port_buffer[2];
} SSD1306_t;

/* Estrutura para a seleção de cores do display (define se o pixel será aceso ou apagado) */
typedef enum {
    SSD1306_COLOR_BLACK = 0x00, // cor preta para apagado
    SSD1306_COLOR_WHITE = 0x01 // cor branca para aceso
} SSD1306_COLOR_t;

/* - Função para cálculo do quanto do buffer será destinado a renderização 
   - Utiliza como parâmetro a estrutura de área de renderização
   - Deve ser utilizada ao iniciar o display           */
void Calculate_RenderArea_BufferLength(struct RenderArea *area);

/* - Função para enviar comandos ao display 
   - Utiliza como parâmetro o comando que deseja ser passado */
void SSD1306_SendCommand(uint8_t command);

/* - Função para enviar uma lista de comandos ao display
   - Utiliza como parâmetro o buffer e o número de comandos a serem passados */
void SSD1306_SendCommand_List(uint8_t *ssd, int number);

/* - Função que envia dados para o display 
   - Utiliza como parâmetro o buffer e o seu respectivo comprimento */
void SSD1306_SendBuffer(uint8_t ssd[], int buffer_length);

/* - Função de inicialização do display */
void SSD1306_Init();

/* - Função para a renderização do display
   - Utiliza como parâmetros o buffer e a estrutura de área de renderização
   - Deve ser utilizada sempre que for desejado atualizar, ou seja, desenhar no display */
void SSD1306_Render(uint8_t *ssd, struct RenderArea *area);

/* - Função para controlar um pixel desejado
   - Utiliza como parâmetros o buffer, as coordenadas cartesianas X e Y do display e a cor desejada (aceso/apagado) */
void SSD1306_SetPixel(uint8_t *ssd, int X, int Y, SSD1306_COLOR_t color);

/* - Função para desenhar uma linha
   - Utiliza como parâmetros o buffer, as coordenadas cartesianas do ponto inicial X0 e Y0, do ponto final X1 e Y1 e a cor desejada (aceso/apagado) */
void SSD1306_DrawLine(uint8_t *ssd, int X0, int Y0, int X1, int Y1, SSD1306_COLOR_t color);

/* - Função para desenhar um retângulo
   - Utiliza como parâmetros o buffer, as coordenadas cartesianas do ponto inicial X e Y, o comprimento W, a altura H e a cor desejada (aceso/apagado) */
void SSD1306_DrawRectangle(uint8_t *ssd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t color);

/* - Função para desenhar um retângulo preenchido 
   - Utiliza como parâmetros o buffer, as coordenadas cartesianas do ponto inicial X e Y, o comprimento W, a altura H e a cor desejada (aceso/apagado) */
void SSD1306_DrawFilledRectangle(uint8_t *ssd, uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t color);

/* - Função para desenhar um triângulo 
   - Utiliza como parâmetros o buffer, cada vértice nas coordenadas cartesianas e a cor desejada (aceso/apagado) */
void SSD1306_DrawTriangle(uint8_t *ssd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color);

/* - Função para desenhar um triângulo preenchido
   - Utiliza como parâmetros o buffer, cada vértice nas coordenadas cartesianas e a cor desejada (aceso/apagado) */
void SSD1306_DrawFilledTriangle(uint8_t *ssd, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color);

/* - Função para desenhar um círculo
   - Utiliza como parâmetros o buffer, as coordenadas cartesianas X e Y do centro, o raio R e a cor desejada (aceso/apagado) */
void SSD1306_DrawCircle(uint8_t *ssd, int16_t x_center, int16_t y_center, int16_t radius, SSD1306_COLOR_t color);

/* - Função para desenhar um círculo preenchido
   - Utiliza como parâmetros o buffer, as coordenadas cartesianas X e Y do centro, o raio R e a cor desejada (aceso/apagado) */
void SSD1306_DrawFilledCircle(int8_t *ssd, int16_t x_center, int16_t y_center, int16_t radius, SSD1306_COLOR_t color);

/* - Função para desenhar um arco
   - Utiliza como parâmetros o buffer, as coordenadas cartesianas do centro X e Y, o raio R, os ângulos de início e fim e a cor desejada (aceso/apagado) */
void SSD1306_DrawArc(uint8_t *ssd, uint16_t x_center, uint16_t y_center, uint16_t radius, uint16_t start_angle, uint16_t end_angle, SSD1306_COLOR_t color);

/* - Função para desenhar um caractere
   - Utiliza como parâmetros o buffer, as coordenadas cartesianas em que se deseja desenhar, o caractere a ser desenhado, o tamanho da fonte de acordo com arquivo Fonts
   e a cor desejada (aceso/apagado) */
void SSD1306_SetChar(uint8_t *ssd, int16_t x, int16_t y, uint8_t character, FontDef_t* Font, SSD1306_COLOR_t color);

/* - Função para desenhar uma string 
   - Utiliza como parâmetros o buffer, as coordenadas cartesianas em que se deseja desenhar, a string a ser desenhada, o tamanho da fonte de acordo com o arquivo Fonts
   e a cor desejada (aceso/apagado) */
void SSD1306_SetString(uint8_t *ssd, int16_t x, int16_t y, const char *str, FontDef_t* Font, SSD1306_COLOR_t color);

/* - Função para desenhar uma string com quebra de linha
   - Utiliza como parâmetros o buffer, as coordenadas cartesianas em que se deseja desenhar, a string a ser desenhada, o tamanho da fonte de acordo com o arquivo Fonts
   e a cor desejada (aceso/apagado)*/
void SSD1306_SetStringWrapped(uint8_t *ssd, int16_t x, int16_t y, const char *str, FontDef_t* Font, SSD1306_COLOR_t color);

/* - Função que limpa o display
   - Utiliza como parâmetros o buffer e a estrutura da área de renderização
   - Não é necessário utilizar a função SSD1306_Render após o seu uso */
void SSD1306_Clear(uint8_t *ssd, struct RenderArea *area);

void SSD1306_Invert(bool invert);

void SSD1306_ON();

void SSD1306_OFF();

#endif