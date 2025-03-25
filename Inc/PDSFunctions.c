#include "PDSFunctions.h"

void convert_to_complex(float *real_buffer, complex double *complex_buffer, int N) {
    for (int i = 0; i < N; i++) {
        complex_buffer[i] = real_buffer[i] + 0.0 * I; // Inicializa a parte imaginária como 0
    }
}

const float filter_coefficients[FILTER_TAPS] = {
    -8.22227103e-05, -8.54656012e-04, -2.07754600e-03, -3.91885692e-03,
    -6.12657310e-03, -7.90410010e-03, -7.96574527e-03, -4.78882189e-03,
     2.98462800e-03,  1.60982695e-02,  3.43137109e-02,  5.62444444e-02,
     7.94645152e-02,  1.00892883e-01,  1.17377383e-01,  1.26342688e-01,
     1.26342688e-01,  1.17377383e-01,  1.00892883e-01,  7.94645152e-02,
     5.62444444e-02,  3.43137109e-02,  1.60982695e-02,  2.98462800e-03,
    -4.78882189e-03, -7.96574527e-03, -7.90410010e-03, -6.12657310e-03,
    -3.91885692e-03, -2.07754600e-03, -8.54656012e-04, -8.22227103e-05
};


void apply_low_pass_filter(const uint16_t *input_buffer, float *filtered_buffer, int size) { 
    for (int i = 0; i < size; i++) { 
        filtered_buffer[i] = 0; // Inicializa a saída para acumuladores 
        for (int j = 0; j < FILTER_TAPS; j++) { 
            int index = i - j; 
            if (index >= 0) { // Apenas soma se o índice estiver dentro dos limites 
                filtered_buffer[i] += input_buffer[index] * filter_coefficients[j]; 
            } 
        } 
    } 
}


void fft_radix2(complex double *x, int N) {
    // Caso base: se o tamanho da FFT for 1, não há mais subdivisão
    if (N <= 1) return;

    // Alocação dinâmica para vetores das partes par e ímpar
    complex double *X_even = (complex double*)malloc(N / 2 * sizeof(complex double));
    complex double *X_odd = (complex double*)malloc(N / 2 * sizeof(complex double));

    // Divisão dos dados em partes par e ímpar
    for (int i = 0; i < N / 2; i++) {
        X_even[i] = x[2 * i];       // Valores com índice par
        X_odd[i] = x[2 * i + 1];   // Valores com índice ímpar
    }

    // Chamadas recursivas para calcular a FFT das partes menores
    fft_radix2(X_even, N / 2);
    fft_radix2(X_odd, N / 2);

    // Combinação das partes para calcular a FFT completa
    for (int k = 0; k < N / 2; k++) {
        // Fator de combinação usando o expoente complexo
        complex double factor = cexp(-2.0 * I * M_PI * k / N) * X_odd[k];
        x[k] = X_even[k] + factor;       // FFT para índice k
        x[k + N / 2] = X_even[k] - factor; // FFT para índice k + N/2
    }

    // Liberação da memória alocada dinamicamente
    free(X_even);
    free(X_odd);
}

