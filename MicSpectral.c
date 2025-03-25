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

int main()
{
    stdio_init_all();

    uint16_t raw_buffer[BUFFER_SIZE];        // Dados brutos do microfone
    float filtered_buffer[BUFFER_SIZE];     // Dados filtrados
    _Complex double complex_buffer[BUFFER_SIZE]; // Dados no domínio complexo
    double magnitude_buffer[BUFFER_SIZE / 2];    // Magnitude do espectro FFT

    // Buffer e área de renderização do display
    uint8_t display_buffer[SSD1306_BufferLength]; // Buffer para o display
    struct RenderArea render_area = {0, SSD1306_WIDTH - 1, 0, SSD1306_NumberOfPages - 1};

    // Inicialize o microfone
    Mic_InitDMA();             // Inicializa o microfone com DMA
    Mic_SampleRate(22000);     // Define a taxa de amostragem para 22 kHz

    // Inicialize o display SSD1306
    i2c_init(I2C_PORT, 400 * 1000); // Inicializa o barramento I2C a 400kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Configura o SDA
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Configura o SCL
    gpio_pull_up(I2C_SDA); // Pull-up no SDA
    gpio_pull_up(I2C_SCL); // Pull-up no SCL

    SSD1306_Init();
    SSD1306_Clear(display_buffer, &render_area);
    SSD1306_Render(display_buffer, &render_area);
    SSD1306_SetString(display_buffer, 0, 0, "Teste", &Font_7x10, SSD1306_COLOR_WHITE);
    SSD1306_Render(display_buffer, &render_area);

    while (1) {

        /*
        
        // Capture os dados do microfone
        Mic_ReadBufferDMA(raw_buffer, BUFFER_SIZE);

        // Aplique o filtro passa-baixa para capturar apenas frequências relevantes
        apply_low_pass_filter(raw_buffer, filtered_buffer, BUFFER_SIZE);

        // Converta os dados filtrados para o domínio complexo
        convert_to_complex(filtered_buffer, complex_buffer, BUFFER_SIZE);

        // Execute a FFT no buffer complexo
        fft_radix2(complex_buffer, BUFFER_SIZE);

        // Calcule as magnitudes e normalize
        double max_magnitude = 0;
        for (int i = 0; i < BUFFER_SIZE / 2; i++) {
            magnitude_buffer[i] = cabs(complex_buffer[i]); // Calcula a magnitude
            if (magnitude_buffer[i] > max_magnitude) {
                max_magnitude = magnitude_buffer[i];
            }
        }
        for (int i = 0; i < BUFFER_SIZE / 2; i++) {
            magnitude_buffer[i] = (magnitude_buffer[i] / max_magnitude) * SSD1306_HEIGHT;
        }

        // Limpe o buffer de display
        SSD1306_Clear(display_buffer, &render_area);

        // Desenhe o espectro no display
        for (int i = 0; i < SSD1306_WIDTH / BAR_WIDTH; i++) {
            int bar_height = (int)magnitude_buffer[i * (BUFFER_SIZE / 2) / (SSD1306_WIDTH / BAR_WIDTH)];
            SSD1306_DrawFilledRectangle(display_buffer, i * BAR_WIDTH, SSD1306_HEIGHT - bar_height, BAR_WIDTH, bar_height, SSD1306_COLOR_WHITE);
        }

        // Atualize o display
        SSD1306_Render(display_buffer, &render_area);*/

        sleep_ms(100); // Pausa para permitir a renderização
    }

    return 0;
}


