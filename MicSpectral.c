#include <stdio.h>
#include "pico/stdlib.h"
#include "microphone.h"
#include "PDSFunctions.h"
#include "Fonts.h"
#include "DisplaySSD1306.h"

#define BUFFER_SIZE 1024
#define BAR_WIDTH 2 // Largura das barras no espectro
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15

// Limitar para faixa de voz humana (85 Hz a 255 Hz)
int start_index = 40; // Índice mínimo para 80 Hz
int end_index = 104;  // Índice máximo para 300 Hz


uint8_t display_buffer[SSD1306_BufferLength]; // Buffer para o display
struct RenderArea render_area = {0, SSD1306_WIDTH - 1, 0, SSD1306_NumberOfPages - 1};

int main()
{
    stdio_init_all();

    uint16_t raw_buffer[BUFFER_SIZE];        // Dados brutos do microfone
    float filtered_buffer[BUFFER_SIZE];     // Dados filtrados
    _Complex double complex_buffer[BUFFER_SIZE]; // Dados no domínio complexo
    double magnitude_buffer[BUFFER_SIZE / 2];    // Magnitude do espectro FFT
    
    float converted_buffer[BUFFER_SIZE];

    // Buffer e área de renderização do display
    

    // Inicialize o microfone
    Mic_InitDMA();             // Inicializa o microfone com DMA
    Mic_SampleRate(5000);     // Define a taxa de amostragem para 22 kHz

    // Inicialize o display SSD1306
    i2c_init(I2C_PORT, 400 * 1000); // Inicializa o barramento I2C a 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Configura o SDA
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Configura o SCL
    gpio_pull_up(I2C_SDA); // Pull-up no SDA
    gpio_pull_up(I2C_SCL); // Pull-up no SCL

    Calculate_RenderArea_BufferLength(&render_area);
    SSD1306_Init();
    
    SSD1306_Clear(display_buffer, &render_area);
    SSD1306_Render(display_buffer, &render_area);
    

    while (1) {

        
        
        // Capture os dados do microfone
        Mic_ReadBufferDMA(raw_buffer, BUFFER_SIZE);

        // Aplique o filtro passa-baixa para capturar apenas frequências relevantes
        apply_low_pass_filter(raw_buffer, filtered_buffer, BUFFER_SIZE);

        // Converta os dados filtrados para o domínio complexo
        convert_to_complex(filtered_buffer, complex_buffer, BUFFER_SIZE);
        /*
        for (int i = 0; i < BUFFER_SIZE; i++) {
            converted_buffer[i] = (float)raw_buffer[i];
        }
        convert_to_complex(converted_buffer, complex_buffer, BUFFER_SIZE);*/

        // Execute a FFT no buffer complexo
        fft_radix2(complex_buffer, BUFFER_SIZE);

        // Calcule as magnitudes e normalize
        double max_magnitude = 0;
        for (int i = 0; i < BUFFER_SIZE / 2; i++) {
            magnitude_buffer[i] = cabs(complex_buffer[i]);
            if (magnitude_buffer[i] > max_magnitude) {
                max_magnitude = magnitude_buffer[i];
            }
        }
        if (max_magnitude > 0) {
            for (int i = 0; i < BUFFER_SIZE / 2; i++) {
                magnitude_buffer[i] = (magnitude_buffer[i] / max_magnitude) * 8*SSD1306_HEIGHT;
            }
        }

        
        int bar_count = end_index - start_index + 1; // Total de barras (11)
        // Limpe o buffer do display
        SSD1306_Clear(display_buffer, &render_area);

        // Desenhe as barras no display
        for (int i = 0; i < bar_count; i++) {
            int index = start_index + i;             // Índice do buffer de magnitude
            int bar_height = (int)magnitude_buffer[index]; // Altura normalizada
            if (bar_height > SSD1306_HEIGHT) {
                bar_height = SSD1306_HEIGHT; // Limitar ao tamanho máximo
            }

            // Desenhar a barra correspondente
            SSD1306_DrawFilledRectangle(
                display_buffer, 
                i * BAR_WIDTH,                // Posição horizontal (x)
                SSD1306_HEIGHT - bar_height, // Posição vertical (y)
                BAR_WIDTH,                   // Largura da barra
                bar_height,                  // Altura da barra
                SSD1306_COLOR_WHITE          // Cor da barra
            );
}

        // Atualize o display
        SSD1306_Render(display_buffer, &render_area);

        sleep_ms(50); // Pausa para renderização
    }

    return 0;
}


