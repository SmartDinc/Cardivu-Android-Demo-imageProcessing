#include "real_double_fft.h"

#include <stdexcept>


RealDoubleFFT::RealDoubleFFT(int _n)
{
    ndim = _n;
    if(wave_table == 0 || wave_table_len != (2*ndim+15)) {
        wave_table_len = 2 * ndim + 15;
        wave_table = new double[wave_table_len];
    }
    rffti(ndim, wave_table);
    ch = new double[_n];
}

RealDoubleFFT::~RealDoubleFFT()
{
    delete wave_table;
    delete ch;
}

void RealDoubleFFT::ft(double* _x, int _length)
{
    if(_length != ndim)
        throw std::invalid_argument("RealDoubleFFT::ft(): the length of data can not match that of the wave_table.");
    rfftf(ndim, _x, wave_table, ch);
}

void RealDoubleFFT::fft_to_amp(double* _ps, double* _x, int _length)
{
    // data.length should be even number
    double scaler = 2.0 / (_length * _length);  // *2 since there are positive and negative frequency part
    _ps[0] = _x[0] * _x[0] * scaler;
    for (int i = 1, j = 1; i < _length - 1; i += 2, j++) {
        _ps[j] = (_x[i] * _x[i] + _x[i + 1] * _x[i + 1]) * scaler;
    }
}
