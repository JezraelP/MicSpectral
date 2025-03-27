#ifndef PDSFUNCTIONS_H_
#define PDSFUNCTIONS_H_
#include <math.h>
#include <complex.h>
#include <stdlib.h>
#include <stdint.h>

/** 
 * @brief Número de coeficientes do filtro 
 * Define o número de taps para o filtro digital 
 */
#define FILTER_TAPS 32

/** 
 * @brief Habilita definições matemáticas 
 * Permite o uso de constantes matemáticas padrão 
 */
#define USE_MATH_DEFINES

/** 
 * @brief Definição de constante PI
 * Fornece o valor de PI caso não esteja definido previamente 
 */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/** 
 * @brief Coeficientes do filtro digital
 * Array de coeficientes pré-calculados para filtragem 
 */
extern const float filter_coefficients[FILTER_TAPS];

/**
 * @brief Converte buffer de números reais para números complexos
 * 
 * Transforma um buffer de valores reais em um buffer de números complexos,
 * útil para processamento de sinais que requerem representação complexa.
 * 
 * @param real_buffer Buffer de entrada com valores reais
 * @param buffer Buffer de saída com números complexos
 * @param N Número de elementos no buffer
 */
void convert_to_complex(float *real_buffer, double complex *buffer, int N);

/**
 * @brief Aplica um filtro passa-baixa ao buffer de entrada
 * 
 * Realiza a filtragem digital de um sinal usando os coeficientes 
 * pré-definidos, atenuando frequências acima de um certo limite.
 * 
 * @param input_buffer Buffer de entrada com dados originais
 * @param filtered_buffer Buffer de saída com dados filtrados
 * @param size Tamanho dos buffers de entrada e saída
 */
void apply_low_pass_filter(const uint16_t *input_buffer, float *filtered_buffer, int size);

/**
 * @brief Implementa a Transformada Rápida de Fourier (FFT) 
 * 
 * Calcula a FFT usando o algoritmo de Radix-2, permitindo 
 * a análise de frequências de um sinal no domínio complexo.
 * 
 * @param x Buffer de números complexos a serem transformados
 * @param N Tamanho do buffer (deve ser potência de 2)
 */
void fft_radix2(_Complex double *x, int N);

#endif // PDSFUNCTIONS_H_