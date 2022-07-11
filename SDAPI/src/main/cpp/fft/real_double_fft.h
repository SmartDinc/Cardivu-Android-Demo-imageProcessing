#ifndef ANDROID_DSP_SERVICE_REAL_DOUBLE_FFT_H
#define ANDROID_DSP_SERVICE_REAL_DOUBLE_FFT_H

#include <cstdlib>
#include "real_double_fft_mixed.h"


class RealDoubleFFT : RealDoubleFFTMixed {

private:
    double* wave_table = NULL;
    double* ch;  // reusable work array
    int wave_table_len;
    int ndim;

public:
    RealDoubleFFT(int _n);
    ~RealDoubleFFT();

public:
    void ft(double* _x, int _length);
    void fft_to_amp(double* data_out, double* data, int _length);

};


#endif //ANDROID_DSP_SERVICE_REAL_DOUBLE_FFT_H
