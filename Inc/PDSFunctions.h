#ifndef _PDSFUNCTIONS_H_
#define _PDSFUNCTIONS_H_

#include <math.h> 
#include <complex.h> 
#include <stdlib.h> 
#include <stdint.h>

#define FILTER_TAPS 32
#define _USE_MATH_DEFINES

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

extern const float filter_coefficients[FILTER_TAPS];

void convert_to_complex(float *real_buffer, _Complex double *complex_buffer, int N);
void apply_low_pass_filter(const uint16_t *input_buffer, float *filtered_buffer, int size);
void fft_radix2(_Complex double *x, int N);

#endif // _PDSFUNCTIONS_H_