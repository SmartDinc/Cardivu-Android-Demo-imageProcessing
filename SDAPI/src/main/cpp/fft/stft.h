#ifndef ANDROID_DSP_SERVICE_STFT_H
#define ANDROID_DSP_SERVICE_STFT_H

#include "real_double_fft.h"
#include "../iir/dsp.h"

class STFT {

/**
 * FFT 관련 변수 및 함수 선언
 */
private:
    double* spectrum_amp_in;
    double* spectrum_amp_in_tmp;
    double* spectrum_amp_out_cum;
    double* spectrum_amp_out_tmp;
    double* spectrum_amp_out_db;
    int sampling_rate;
    int in_len;
    int out_len;
    int spectrum_amp_pt;
    int n_elem_spectrum_amp = 0;
    RealDoubleFFT* spectrum_amp_fft;

public:
    STFT(int _fft_len, int _sampling_rate);
    ~STFT();

private:
    void init(int _fft_len, int _sampling_rate);
    void fftToAmp(double* data_out, double* data, int _length);

public:
    void feedData(short* _ds, int _ds_len);
    const double* getSpectrumAmpDB(int& _out_len);
    int get_n_elem_spectrum_amp();
    void clear();

//------------------------------------------------------------------------------

/**
 * Bandpass Filter 관련 변수 및 함수 선언
 */
private:
    bool pre_processing = false;
    Dsp::Filter* filter = NULL;

private:
    void initDspFilter(int _fft_len, int _sampling_rate, int _order, int _low, int _high);
    void bandpass(double* data_out, double* data, int _length);

public:
    void set_pre_processing(bool _pre_processing);

};


#endif //ANDROID_DSP_SERVICE_STFT_H
