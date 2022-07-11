#include "real_double_fft_mixed.h"

#include <iostream>
#include <cmath>

#define PI  3.141592653589793238460

static const int ntryh[] = {4, 2, 3, 5};


void RealDoubleFFTMixed::radf2(int _ido, int _l1, const double* _cc, double* _ch, const double* _wtable, int _offset)
{
    int     i, k, ic;
    double  ti2, tr2;
    int iw1;
    iw1 = _offset;

    for(k=0; k<_l1; k++) {
        _ch[2*k*_ido] = _cc[k*_ido] + _cc[(k+_l1)*_ido];
        _ch[(2*k+1)*_ido+_ido-1] = _cc[k*_ido] - _cc[(k+_l1) * _ido];
    }
    if(_ido < 2) return;
    if(_ido != 2) {
        for(k=0; k<_l1; k++) {
            for(i=2; i<_ido; i+=2) {
                ic = _ido - i;
                tr2 = _wtable[i-2+iw1] * _cc[i-1+(k+_l1)*_ido]
                      + _wtable[i-1+iw1] * _cc[i+(k+_l1)*_ido];
                ti2 = _wtable[i-2+iw1] * _cc[i+(k+_l1)*_ido]
                      -_wtable[i-1+iw1] * _cc[i-1+(k+_l1)*_ido];
                _ch[i+2*k*_ido] = _cc[i+k*_ido] + ti2;
                _ch[ic+(2*k+1)*_ido] = ti2 - _cc[i+k*_ido];
                _ch[i-1+2*k*_ido] = _cc[i-1+k*_ido] + tr2;
                _ch[ic-1+(2*k+1)*_ido] = _cc[i-1+k*_ido] - tr2;
            }
        }
        if(_ido % 2 == 1) return;
    }
    for(k=0; k<_l1; k++) {
        _ch[(2*k+1)*_ido] -= _cc[_ido-1+(k+_l1)*_ido];
        _ch[_ido-1+2*k*_ido] = _cc[_ido-1+k*_ido];
    }
}

void RealDoubleFFTMixed::radf3(int _ido, int _l1, const double* _cc, double* _ch, const double* _wtable, int _offset)
{
    const double taur = -0.5;
    const double taui = 0.866025403784439;
    int     i, k, ic;
    double  ci2, di2, di3, cr2, dr2, dr3, ti2, ti3, tr2, tr3;
    int iw1, iw2;
    iw1 = _offset;
    iw2 = iw1 + _ido;

    for(k=0; k<_l1; k++) {
        cr2 = _cc[(k+_l1)*_ido] + _cc[(k+2*_l1)*_ido];
        _ch[3*k*_ido] = _cc[k*_ido] + cr2;
        _ch[(3*k+2)*_ido] = taui*(_cc[(k+_l1*2)*_ido] - _cc[(k+_l1)*_ido]);
        _ch[_ido-1+(3*k+1)*_ido] = _cc[k*_ido] + taur * cr2;
    }
    if(_ido == 1) return;
    for(k=0; k<_l1; k++) {
        for(i=2; i<_ido; i+=2) {
            ic = _ido - i;
            dr2 = _wtable[i-2+iw1] * _cc[i-1+(k+_l1)*_ido]
                  + _wtable[i-1+iw1] * _cc[i+(k+_l1)*_ido];
            di2 = _wtable[i-2+iw1] * _cc[i+(k+_l1)*_ido]
                  - _wtable[i-1+iw1] * _cc[i-1+(k+_l1)*_ido];
            dr3 = _wtable[i-2+iw2] * _cc[i-1+(k+_l1*2)*_ido]
                  + _wtable[i-1+iw2] * _cc[i+(k+_l1*2)*_ido];
            di3 = _wtable[i-2+iw2] * _cc[i+(k+_l1*2)*_ido]
                  - _wtable[i-1+iw2] * _cc[i-1+(k+_l1*2)*_ido];
            cr2 = dr2 + dr3;
            ci2 = di2 + di3;
            _ch[i-1+3*k*_ido] = _cc[i-1+k*_ido] + cr2;
            _ch[i+3*k*_ido] = _cc[i+k*_ido] + ci2;
            tr2 = _cc[i-1+k*_ido] + taur * cr2;
            ti2 = _cc[i+k*_ido] + taur * ci2;
            tr3 = taui * (di2 - di3);
            ti3 = taui * (dr3 - dr2);
            _ch[i-1+(3*k+2)*_ido] = tr2+tr3;
            _ch[ic-1+(3*k+1)*_ido] = tr2-tr3;
            _ch[i+(3*k+2)*_ido] = ti2+ti3;
            _ch[ic+(3*k+1)*_ido] = ti3-ti2;
        }
    }
}

void RealDoubleFFTMixed::radf4(int _ido, int _l1, const double* _cc, double* _ch, const double* _wtable, int _offset)
{
    const double hsqt2 = 0.7071067811865475;
    int i, k, ic;
    double  ci2, ci3, ci4, cr2, cr3, cr4, ti1, ti2, ti3, ti4, tr1, tr2, tr3, tr4;
    int iw1, iw2, iw3;
    iw1 = _offset;
    iw2 = _offset + _ido;
    iw3 = iw2 + _ido;
    for(k=0; k<_l1; k++) {
        tr1 = _cc[(k+_l1)*_ido] + _cc[(k+3*_l1)*_ido];
        tr2 = _cc[k*_ido] + _cc[(k+2*_l1)*_ido];
        _ch[4*k*_ido] = tr1 + tr2;
        _ch[_ido-1+(4*k+3)*_ido] = tr2 - tr1;
        _ch[_ido-1+(4*k+1)*_ido] = _cc[k*_ido] - _cc[(k+2*_l1)*_ido];
        _ch[(4*k+2)*_ido] = _cc[(k+3*_l1)*_ido] - _cc[(k+_l1)*_ido];
    }
    if(_ido < 2) return;
    if(_ido != 2) {
        for(k=0; k<_l1; k++) {
            for(i=2; i<_ido; i+=2) {
                ic = _ido - i;
                cr2 = _wtable[i-2+iw1] * _cc[i-1+(k+_l1)*_ido]
                      + _wtable[i-1+iw1] * _cc[i+(k+_l1)*_ido];
                ci2 = _wtable[i-2+iw1] * _cc[i+(k+_l1)*_ido]
                      - _wtable[i-1+iw1] * _cc[i-1+(k+_l1)*_ido];
                cr3 = _wtable[i-2+iw2] * _cc[i-1+(k+2*_l1)*_ido]
                      + _wtable[i-1+iw2] * _cc[i+(k+2*_l1)*_ido];
                ci3 =  _wtable[i-2+iw2] * _cc[i+(k+2*_l1)*_ido]
                       - _wtable[i-1+iw2] * _cc[i-1+(k+2*_l1)*_ido];
                cr4 = _wtable[i-2+iw3] * _cc[i-1+(k+3*_l1)*_ido]
                      + _wtable[i-1+iw3] * _cc[i+(k+3*_l1)*_ido];
                ci4 = _wtable[i-2+iw3] * _cc[i+(k+3*_l1)*_ido]
                      - _wtable[i-1+iw3] * _cc[i-1+(k+3*_l1)*_ido];
                tr1 = cr2 + cr4;
                tr4 = cr4 - cr2;
                ti1 = ci2 + ci4;
                ti4 = ci2 - ci4;
                ti2 = _cc[i+k*_ido] + ci3;
                ti3 = _cc[i+k*_ido] - ci3;
                tr2 = _cc[i-1+k*_ido] + cr3;
                tr3 = _cc[i-1+k*_ido] - cr3;
                _ch[i-1+4*k*_ido] = tr1 + tr2;
                _ch[ic-1+(4*k+3)*_ido] = tr2 - tr1;
                _ch[i+4*k*_ido] = ti1 + ti2;
                _ch[ic+(4*k+3)*_ido] = ti1 - ti2;
                _ch[i-1+(4*k+2)*_ido] = ti4 + tr3;
                _ch[ic-1+(4*k+1)*_ido] = tr3 - ti4;
                _ch[i+(4*k+2)*_ido] = tr4 + ti3;
                _ch[ic+(4*k+1)*_ido] = tr4 - ti3;
            }
        }
        if(_ido%2 == 1) return;
    }
    for(k=0; k<_l1; k++) {
        ti1 =- hsqt2 * (_cc[_ido-1+(k+_l1)*_ido] + _cc[_ido-1+(k+3*_l1)*_ido]);
        tr1 = hsqt2 * (_cc[_ido-1+(k+_l1)*_ido] - _cc[_ido-1+(k+3*_l1)*_ido]);
        _ch[_ido-1+4*k*_ido] = tr1 + _cc[_ido-1+k*_ido];
        _ch[_ido-1+(4*k+2)*_ido] = _cc[_ido-1+k*_ido] - tr1;
        _ch[(4*k+1)*_ido] = ti1 - _cc[_ido-1+(k+2*_l1)*_ido];
        _ch[(4*k+3)*_ido] = ti1 + _cc[_ido-1+(k+2*_l1)*_ido];
    }
}

void RealDoubleFFTMixed::radf5(int _ido, int _l1, const double* _cc, double* _ch, const double* _wtable, int _offset)
{
    const double tr11 = 0.309016994374947;
    const double ti11 = 0.951056516295154;
    const double tr12 = -0.809016994374947;
    const double ti12 = 0.587785252292473;
    int     i, k, ic;
    double  ci2, di2, ci4, ci5, di3, di4, di5, ci3, cr2, cr3, dr2, dr3,
            dr4, dr5, cr5, cr4, ti2, ti3, ti5, ti4, tr2, tr3, tr4, tr5;
    int iw1, iw2, iw3, iw4;
    iw1 = _offset;
    iw2 = iw1 + _ido;
    iw3 = iw2 + _ido;
    iw4 = iw3 + _ido;

    for(k=0; k<_l1; k++) {
        cr2 = _cc[(k+4*_l1)*_ido] + _cc[(k+_l1)*_ido];
        ci5 = _cc[(k+4*_l1)*_ido] - _cc[(k+_l1)*_ido];
        cr3 = _cc[(k+3*_l1)*_ido] + _cc[(k+2*_l1)*_ido];
        ci4=  _cc[(k+3*_l1)*_ido] - _cc[(k+2*_l1)*_ido];
        _ch[5*k*_ido] = _cc[k*_ido] + cr2 + cr3;
        _ch[_ido-1+(5*k+1)*_ido] = _cc[k*_ido] + tr11 * cr2 + tr12 * cr3;
        _ch[(5*k+2)*_ido] = ti11 * ci5 + ti12 * ci4;
        _ch[_ido-1+(5*k+3)*_ido] = _cc[k*_ido] + tr12 * cr2 + tr11 * cr3;
        _ch[(5*k+4)*_ido] = ti12 * ci5 - ti11 * ci4;
    }
    if(_ido == 1) return;
    for(k=0; k<_l1;++k) {
        for(i=2; i<_ido; i+=2) {
            ic = _ido - i;
            dr2 = _wtable[i-2+iw1] * _cc[i-1+(k+_l1)*_ido]
                  + _wtable[i-1+iw1] * _cc[i+(k+_l1)*_ido];
            di2 = _wtable[i-2+iw1] * _cc[i+(k+_l1)*_ido]
                  - _wtable[i-1+iw1] * _cc[i-1+(k+_l1)*_ido];
            dr3 = _wtable[i-2+iw2] * _cc[i-1+(k+2*_l1)*_ido]
                  + _wtable[i-1+iw2] * _cc[i+(k+2*_l1)*_ido];
            di3 = _wtable[i-2+iw2] * _cc[i+(k+2*_l1)*_ido]
                  - _wtable[i-1+iw2] * _cc[i-1+(k+2*_l1)*_ido];
            dr4 = _wtable[i-2+iw3] * _cc[i-1+(k+3*_l1)*_ido]
                  + _wtable[i-1+iw3] * _cc[i+(k+3*_l1)*_ido];
            di4 = _wtable[i-2+iw3] * _cc[i+(k+3*_l1)*_ido]
                  - _wtable[i-1+iw3] * _cc[i-1+(k+3*_l1)*_ido];
            dr5 = _wtable[i-2+iw4] * _cc[i-1+(k+4*_l1)*_ido]
                  + _wtable[i-1+iw4] * _cc[i+(k+4*_l1)*_ido];
            di5 = _wtable[i-2+iw4] * _cc[i+(k+4*_l1)*_ido]
                  - _wtable[i-1+iw4] * _cc[i-1+(k+4*_l1)*_ido];
            cr2 = dr2 + dr5;
            ci5 = dr5 - dr2;
            cr5 = di2 - di5;
            ci2 = di2 + di5;
            cr3 = dr3 + dr4;
            ci4 = dr4 - dr3;
            cr4 = di3 - di4;
            ci3 = di3 + di4;
            _ch[i-1+5*k*_ido] = _cc[i-1+k*_ido] + cr2 + cr3;
            _ch[i+5*k*_ido] = _cc[i+k*_ido] + ci2 + ci3;
            tr2 = _cc[i-1+k*_ido] + tr11 * cr2 + tr12 * cr3;
            ti2 = _cc[i+k*_ido] + tr11 * ci2 + tr12 * ci3;
            tr3 = _cc[i-1+k*_ido] + tr12 * cr2 + tr11 * cr3;
            ti3 = _cc[i+k*_ido] + tr12 * ci2 + tr11 * ci3;
            tr5 = ti11 * cr5 + ti12 * cr4;
            ti5 = ti11 * ci5 + ti12 * ci4;
            tr4 = ti12 * cr5 - ti11 * cr4;
            ti4 = ti12 * ci5 - ti11 * ci4;
            _ch[i-1+(5*k+2)*_ido] = tr2 + tr5;
            _ch[ic-1+(5*k+1)*_ido] = tr2 - tr5;
            _ch[i+(5*k+2)*_ido] = ti2 + ti5;
            _ch[ic+(5*k+1)*_ido] = ti5 - ti2;
            _ch[i-1+(5*k+4)*_ido] = tr3 + tr4;
            _ch[ic-1+(5*k+3)*_ido] = tr3 - tr4;
            _ch[i+(5*k+4)*_ido] = ti3 + ti4;
            _ch[ic+(5*k+3)*_ido] = ti4 - ti3;
        }
    }
}

void RealDoubleFFTMixed::radfg(int _ido, int _ip, int _l1, int _idl1, double* _cc, double* _c1, double* _c2, double* _ch, double* _ch2, const double* _wtable, int _offset)
{
    const double twopi = 2.0 * PI; //6.28318530717959;
    int     idij, ipph, i, j, k, l, j2, ic, jc, lc, ik, is, nbd;
    double  dc2, ai1, ai2, ar1, ar2, ds2, dcp, arg, dsp, ar1h, ar2h;
    int iw1 = _offset;

    arg = twopi / _ip;
    dcp = cos(arg);
    dsp = sin(arg);
    ipph = (_ip + 1) / 2;
    nbd = (_ido - 1) / 2;
    if(_ido != 1) {
        for(ik=0; ik<_idl1; ik++) _ch2[ik] = _c2[ik];
        for(j=1; j<_ip; j++)
            for(k=0; k<_l1; k++)
                _ch[(k+j*_l1)*_ido] = _c1[(k+j*_l1)*_ido];
        if(nbd <= _l1) {
            is = -_ido;
            for(j=1; j<_ip; j++) {
                is += _ido;
                idij = is - 1;
                for(i=2; i<_ido; i+=2) {
                    idij += 2;
                    for(k=0; k<_l1; k++) {
                        _ch[i-1+(k+j*_l1)*_ido] =
                                _wtable[idij-1+iw1] * _c1[i-1+(k+j*_l1)*_ido]
                                + _wtable[idij+iw1] * _c1[i+(k+j*_l1)*_ido];
                        _ch[i+(k+j*_l1)*_ido] =
                                _wtable[idij-1+iw1] * _c1[i+(k+j*_l1)*_ido]
                                - _wtable[idij+iw1] * _c1[i-1+(k+j*_l1)*_ido];
                    }
                }
            }
        } else {
            is = -_ido;
            for(j=1; j<_ip; j++) {
                is += _ido;
                for(k=0; k<_l1; k++) {
                    idij = is - 1;
                    for(i=2; i<_ido; i+=2) {
                        idij += 2;
                        _ch[i-1+(k+j*_l1)*_ido] =
                                _wtable[idij-1+iw1] * _c1[i-1+(k+j*_l1)*_ido]
                                + _wtable[idij+iw1] * _c1[i+(k+j*_l1)*_ido];
                        _ch[i+(k+j*_l1)*_ido] =
                                _wtable[idij-1+iw1] * _c1[i+(k+j*_l1)*_ido]
                                - _wtable[idij+iw1] * _c1[i-1+(k+j*_l1)*_ido];
                    }
                }
            }
        }
        if(nbd >= _l1) {
            for(j=1; j<ipph; j++) {
                jc = _ip - j;
                for(k=0; k<_l1; k++) {
                    for(i=2; i<_ido; i+=2) {
                        _c1[i-1+(k+j*_l1)*_ido] = _ch[i-1+(k+j*_l1)*_ido] + _ch[i-1+(k+jc*_l1)*_ido];
                        _c1[i-1+(k+jc*_l1)*_ido] = _ch[i+(k+j*_l1)*_ido] - _ch[i+(k+jc*_l1)*_ido];
                        _c1[i+(k+j*_l1)*_ido] = _ch[i+(k+j*_l1)*_ido] + _ch[i+(k+jc*_l1)*_ido];
                        _c1[i+(k+jc*_l1)*_ido] = _ch[i-1+(k+jc*_l1)*_ido] - _ch[i-1+(k+j*_l1)*_ido];
                    }
                }
            }
        } else {
            for(j=1; j<ipph; j++) {
                jc = _ip - j;
                for(i=2; i<_ido; i+=2) {
                    for(k=0; k<_l1; k++) {
                        _c1[i-1+(k+j*_l1)*_ido] =
                                _ch[i-1+(k+j*_l1)*_ido] + _ch[i-1+(k+jc*_l1)*_ido];
                        _c1[i-1+(k+jc*_l1)*_ido] = _ch[i+(k+j*_l1)*_ido] - _ch[i+(k+jc*_l1)*_ido];
                        _c1[i+(k+j*_l1)*_ido] = _ch[i+(k+j*_l1)*_ido] + _ch[i+(k+jc*_l1)*_ido];
                        _c1[i+(k+jc*_l1)*_ido] = _ch[i-1+(k+jc*_l1)*_ido] - _ch[i-1+(k+j*_l1)*_ido];
                    }
                }
            }
        }
    } else {
        for(ik=0; ik<_idl1; ik++) _c2[ik] = _ch2[ik];
    }
    for(j=1; j<ipph; j++) {
        jc = _ip - j;
        for(k=0; k<_l1; k++) {
            _c1[(k+j*_l1)*_ido] = _ch[(k+j*_l1)*_ido] + _ch[(k+jc*_l1)*_ido];
            _c1[(k+jc*_l1)*_ido] = _ch[(k+jc*_l1)*_ido] - _ch[(k+j*_l1)*_ido];
        }
    }

    ar1=1;
    ai1=0;
    for(l=1; l<ipph; l++) {
        lc = _ip - l;
        ar1h = dcp * ar1 - dsp * ai1;
        ai1 = dcp * ai1 + dsp * ar1;
        ar1 = ar1h;
        for(ik=0; ik<_idl1; ik++) {
            _ch2[ik+l*_idl1] = _c2[ik]+ar1 * _c2[ik+_idl1];
            _ch2[ik+lc*_idl1] = ai1 * _c2[ik+(_ip-1)*_idl1];
        }
        dc2 = ar1;
        ds2 = ai1;
        ar2 = ar1;
        ai2 = ai1;
        for(j=2; j<ipph; j++) {
            jc = _ip - j;
            ar2h = dc2 * ar2 - ds2 * ai2;
            ai2 = dc2 * ai2 + ds2 * ar2;
            ar2 = ar2h;
            for(ik=0; ik<_idl1; ik++) {
                _ch2[ik+l*_idl1] += ar2 * _c2[ik+j*_idl1];
                _ch2[ik+lc*_idl1] += ai2 * _c2[ik+jc*_idl1];
            }
        }
    }
    for(j=1; j<ipph; j++)
        for(ik=0; ik<_idl1; ik++)
            _ch2[ik] += _c2[ik+j*_idl1];

    if(_ido >= _l1) {
        for(k=0; k<_l1; k++) {
            for(i=0; i<_ido; i++) {
                _cc[i+k*_ip*_ido] = _ch[i+k*_ido];
            }
        }
    } else {
        for(i=0; i<_ido; i++) {
            for(k=0; k<_l1; k++) {
                _cc[i+k*_ip*_ido] = _ch[i+k*_ido];
            }
        }
    }
    for(j=1; j<ipph; j++) {
        jc = _ip - j;
        j2 = 2 * j;
        for(k=0; k<_l1; k++) {
            _cc[_ido-1+(j2-1+k*_ip)*_ido] = _ch[(k+j*_l1)*_ido];
            _cc[(j2+k*_ip)*_ido] = _ch[(k+jc*_l1)*_ido];
        }
    }
    if(_ido == 1) return;
    if(nbd >= _l1) {
        for(j=1; j<ipph; j++) {
            jc = _ip - j;
            j2 = 2 * j;
            for(k=0; k<_l1; k++) {
                for(i=2; i<_ido; i+=2) {
                    ic = _ido - i;
                    _cc[i-1+(j2+k*_ip)*_ido] = _ch[i-1+(k+j*_l1)*_ido] + _ch[i-1+(k+jc*_l1)*_ido];
                    _cc[ic-1+(j2-1+k*_ip)*_ido] = _ch[i-1+(k+j*_l1)*_ido] - _ch[i-1+(k+jc*_l1)*_ido];
                    _cc[i+(j2+k*_ip)*_ido] = _ch[i+(k+j*_l1)*_ido] + _ch[i+(k+jc*_l1)*_ido];
                    _cc[ic+(j2-1+k*_ip)*_ido] = _ch[i+(k+jc*_l1)*_ido] - _ch[i+(k+j*_l1)*_ido];
                }
            }
        }
    } else {
        for(j=1; j<ipph; j++) {
            jc = _ip - j;
            j2 = 2 * j;
            for(i=2; i<_ido; i+=2) {
                ic = _ido - i;
                for(k=0; k<_l1; k++) {
                    _cc[i-1+(j2+k*_ip)*_ido] = _ch[i-1+(k+j*_l1)*_ido] + _ch[i-1+(k+jc*_l1)*_ido];
                    _cc[ic-1+(j2-1+k*_ip)*_ido] = _ch[i-1+(k+j*_l1)*_ido] - _ch[i-1+(k+jc*_l1)*_ido];
                    _cc[i+(j2+k*_ip)*_ido] = _ch[i+(k+j*_l1)*_ido] + _ch[i+(k+jc*_l1)*_ido];
                    _cc[ic+(j2-1+k*_ip)*_ido] = _ch[i+(k+jc*_l1)*_ido] - _ch[i+(k+j*_l1)*_ido];
                }
            }
        }
    }
}

void RealDoubleFFTMixed::rfftf1(int _n, double* _c, const double* _wtable, int _offset, double* _ch)
{
    int     i;
    int     k1, l1, l2, na, kh, nf, ip, iw, ido, idl1;

    memcpy(_ch, _wtable+_offset, _n*sizeof(double));

    nf = (int) _wtable[1+2*_n+_offset];
    na = 1;
    l2 = _n;
    iw = _n - 1 + _n + _offset;
    for(k1=1; k1<=nf; ++k1) {
        kh = nf - k1;
        ip = (int) _wtable[kh+2+2*_n+_offset];
        l1 = l2 / ip;
        ido = _n / l2;
        idl1 = ido * l1;
        iw -= (ip-1) * ido;
        na = 1 - na;
        if(ip == 4) {
            if(na == 0) {
                radf4(ido, l1, _c, _ch, _wtable, iw);
            } else {
                radf4(ido, l1, _ch, _c, _wtable, iw);
            }
        } else if(ip == 2) {
            if(na == 0) {
                radf2(ido, l1, _c, _ch, _wtable, iw);
            } else {
                radf2(ido, l1, _ch, _c, _wtable, iw);
            }
        } else if(ip == 3) {
            if(na == 0) {
                radf3(ido, l1, _c, _ch,_wtable, iw);
            } else {
                radf3(ido, l1, _ch, _c, _wtable, iw);
            }
        } else if(ip == 5) {
            if(na == 0) {
                radf5(ido, l1, _c, _ch, _wtable, iw);
            } else {
                radf5(ido, l1, _ch, _c, _wtable, iw);
            }
        } else {
            if(ido == 1) na = 1 - na;
            if(na == 0) {
                radfg(ido, ip, l1, idl1, _c, _c, _c, _ch, _ch, _wtable, iw);
                na = 1;
            } else {
                radfg(ido, ip, l1, idl1, _ch, _ch, _ch, _c, _c, _wtable, iw);
                na = 0;
            }
        }
        l2 = l1;
    }
    if(na == 1) return;
    for(i=0; i<_n; i++) _c[i] = _ch[i];
}

void RealDoubleFFTMixed::rfftf(int _n, double* _r, double* _wtable, double* _ch)
{
    if(_n == 1) return;
    rfftf1(_n, _r, _wtable, 0, _ch);
}

void RealDoubleFFTMixed::rffti1(int _n, double* _wtable, int _offset)
{
    const double twopi = 2.0 * PI;
    double  argh;
    int     ntry = 0, i, j;
    double  argld;
    int     k1, l1, l2, ib;
    double  fi;
    int     ld, ii, nf, ip, nl, is, nq, nr;
    double  arg;
    int     ido, ipm;
    int     nfm1;

    nl=_n;
    nf=0;
    j=0;

    factorize_loop:
    while(true) {
        ++j;
        if(j <= 4)
            ntry = ntryh[j-1];
        else
            ntry += 2;
        do {
            nq = nl / ntry;
            nr = nl - ntry * nq;
            if(nr != 0) goto factorize_loop;
            ++nf;
            _wtable[nf+1+2*_n+_offset] = ntry;

            nl = nq;
            if(ntry==2 && nf !=1) {
                for(i=2; i<=nf; i++) {
                    ib = nf - i + 2;
                    _wtable[ib+1+2*_n+_offset] = _wtable[ib+2*_n+_offset];
                }
                _wtable[2+2*_n+_offset] = 2;
            }
        } while(nl !=1);
        break;
    }
    _wtable[0+2*_n+_offset] = _n;
    _wtable[1+2*_n+_offset] = nf;
    argh = twopi /(_n);
    is = 0;
    nfm1 = nf - 1;
    l1 = 1;
    if(nfm1 == 0) return;
    for(k1=1; k1<=nfm1; k1++) {
        ip = (int) _wtable[k1+1+2*_n+_offset];
        ld = 0;
        l2 = l1 * ip;
        ido = _n / l2;
        ipm = ip - 1;
        for(j=1; j<=ipm;++j) {
            ld += l1;
            i = is;
            argld = ld * argh;

            fi = 0;
            for(ii=3; ii<=ido; ii+=2) {
                i += 2;
                fi += 1;
                arg = fi * argld;
                _wtable[i-2+_n+_offset] = cos(arg);
                _wtable[i-1+_n+_offset] = sin(arg);
            }
            is += ido;
        }
        l1 = l2;
    }
}

void RealDoubleFFTMixed::rffti(int _n, double* _wtable)
{
    if(_n == 1) return;
    rffti1(_n, _wtable, 0);
}