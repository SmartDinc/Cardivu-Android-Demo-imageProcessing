#ifndef ANDROID_DSP_SERVICE_REAL_DOUBLE_FFT_MIXED_H
#define ANDROID_DSP_SERVICE_REAL_DOUBLE_FFT_MIXED_H


class RealDoubleFFTMixed {

protected:
    void radf2(int _ido, int _l1, const double* _cc, double* _ch, const double* _wtable, int _offset);  // real FFT's forward processing of factor 2
    void radf3(int _ido, int _l1, const double* _cc, double* _ch, const double* _wtable, int _offset);  // real FFT's forward processing of factor 3
    void radf4(int _ido, int _l1, const double* _cc, double* _ch, const double* _wtable, int _offset);  // real FFT's forward processing of factor 3
    void radf5(int _ido, int _l1, const double* _cc, double* _ch, const double* _wtable, int _offset);  // real FFT's forward processing of factor 3
    void radfg(int _ido, int _ip, int _l1, int _idl1, double* _cc, double* _c1, double* _c2, double* _ch, double* _ch2, const double* _wtable, int _offset);  // real FFT's forward processing of general factor
    void rfftf1(int _n, double* _c, const double* _wtable, int _offset, double* _ch);  // further processing of real forward FFT
    void rfftf(int _n, double* _r, double* _wtable, double* _ch);  // real forward FFT
    void rffti1(int _n, double* _wtable, int _offset);  // further initialization of real FFT
    void rffti(int _n, double* _wtable);  // initialization of real FFT

};


#endif //ANDROID_DSP_SERVICE_REAL_DOUBLE_FFT_MIXED_H
