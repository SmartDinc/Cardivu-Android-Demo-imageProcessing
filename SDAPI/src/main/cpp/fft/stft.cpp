#include "stft.h"
#include <iostream>

/**
 * STFT 클래스 생성자
 *
 * @param _fft_len FFT 입력 신호의 길이
 * @param _sampling_rate FFT 입력 신호의 Sampling Rate
 */
STFT::STFT(int _fft_len, int _sampling_rate)
{
    init(_fft_len, _sampling_rate);
}

/**
 * STFT 클래스 소멸자
 */
STFT::~STFT()
{
    delete spectrum_amp_in;
    delete spectrum_amp_in_tmp;
    delete spectrum_amp_out_cum;
    delete spectrum_amp_out_tmp;
    delete spectrum_amp_out_db;
    delete spectrum_amp_fft;
}

/**
 * 클래스 변수 초기화
 *
 * @param _fft_len FFT 입력 신호의 길이
 * @param _sampling_rate FFT 입력 신호의 Sampling Rate
 */
void STFT::init(int _fft_len, int _sampling_rate)
{
    if (((-_fft_len) & _fft_len) != _fft_len) {
        // error: fftlen should be power of 2
        throw std::invalid_argument("STFT::Init(): currently, only power of 2 are supported in _fft_len.");
    }
    sampling_rate = _sampling_rate;
    in_len = _fft_len;
    out_len = _fft_len / 2 + 1;
    spectrum_amp_in = new double[in_len];
    spectrum_amp_in_tmp = new double[in_len];
    spectrum_amp_out_cum = new double[out_len];
    spectrum_amp_out_tmp = new double[out_len];
    spectrum_amp_out_db = new double[out_len];
    spectrum_amp_fft = new RealDoubleFFT(in_len);

    initDspFilter(in_len, sampling_rate, 2, 18500, 19600);  // 2 order, 18500~19600 hz
    pre_processing = false;

    clear();
}

/**
 * 주파수를 파워 스펙트럼으로 계산
 *
 * @param data_out 출력 파워 스펙트럼
 * @param data 입력 주파수
 * @param _length 입력 주파수의 길이
 */
void STFT::fftToAmp(double* data_out, double* data, int _length)
{
    // data.length should be even number
    double scaler = 2.0 * 2.0 / (_length * _length);  // *2 since there are positive and negative frequency part
    data_out[0] = data[0] * data[0] * scaler / 4.0;
    int j = 1;
    for (int i = 1; i < _length - 1; i += 2, j++) {
        data_out[j] = (data[i] * data[i] + data[i + 1] * data[i + 1]) * scaler;
    }
    data_out[j] = data[_length - 1] * data[_length - 1] * scaler / 4.0;
}

/**
 * FFT를 수행하기 위한 버퍼에 입력 신호를 추가
 *
 * @param _ds  입력 신호
 * @param _ds_len 입력 신호의 길이
 */
void STFT::feedData(short* _ds, int _ds_len)
{
    int ds_pt = 0;  // input data point to be read
    while(ds_pt < _ds_len) {
        while((spectrum_amp_pt < in_len) && (ds_pt < _ds_len)) {
            double s = _ds[ds_pt++] / 32768.0;
            spectrum_amp_in[spectrum_amp_pt++] = s;
        }
        if(spectrum_amp_pt == in_len) {  // enough data for one FFT
            // if pre_processing is true,
            // the bandpass filter is applied for the audio data before FFT.
            if(pre_processing) {
                bandpass(spectrum_amp_in_tmp, spectrum_amp_in, in_len);
            } else {
                memcpy(spectrum_amp_in_tmp, spectrum_amp_in, in_len*sizeof(double));
            }

            spectrum_amp_fft->ft(spectrum_amp_in_tmp, in_len);  // FFT
            fftToAmp(spectrum_amp_out_tmp, spectrum_amp_in_tmp, in_len);  // power spectrum
            for(int i=0; i<out_len; i++) {
                spectrum_amp_out_cum[i] += spectrum_amp_out_tmp[i];
            }
            n_elem_spectrum_amp++;
            // half overlap (set spectrum_amp_pt = 0 for no overlap)
            int n2 = in_len / 2;
            memcpy(spectrum_amp_in, spectrum_amp_in+n2, n2*sizeof(double));
            spectrum_amp_pt = n2;
        }
    }
}

/**
 * 파워 스펙트럼 추출
 *
 * @param _out_len 추출된 파워 스펙트럼의 길이
 * @return 파워 스펙트럼
 */
const double* STFT::getSpectrumAmpDB(int& _out_len)
{
    if(n_elem_spectrum_amp != 0) { // no new result
        _out_len = out_len;
        for(int i=0; i<out_len; i++) {
            double spectrum_amp_out_com_tmp = spectrum_amp_out_cum[i] / n_elem_spectrum_amp;
            spectrum_amp_out_db[i] = 10.0 * log10(spectrum_amp_out_com_tmp);
        }
        n_elem_spectrum_amp = 0;
        memset(spectrum_amp_out_cum, 0, out_len*sizeof(double));
    }
    return spectrum_amp_out_db;
}

/**
 * FFT 분석된 파워 스펙트럼의 갯수 추출
 *
 * @return 파워 스펙트럼의 갯수
 */
int STFT::get_n_elem_spectrum_amp()
{
    return n_elem_spectrum_amp;
}

/**
 * 클래스 변수를 0 값으로 초기화
 */
void STFT::clear()
{
    spectrum_amp_pt = 0;
    memset(spectrum_amp_out_tmp, 0, out_len*sizeof(double));
    memset(spectrum_amp_out_cum, 0, out_len*sizeof(double));
    memset(spectrum_amp_out_db, 0, out_len*sizeof(double));
}

//------------------------------------------------------------------------------

/**
 * Butterworth Bandpass Filter 초기화
 *
 * @param _fft_len 입력 신호의 길이
 * @param _sampling_rate 입력 신호의 Sampling Rate
 * @param _order 필터의 차수
 * @param _low 저주파 통과 대역
 * @param _high 고주파 통과 대역
 */
void STFT::initDspFilter(int _fft_len, int _sampling_rate, int _order, int _low, int _high)
{
    // calculate parameters for bandpass
    int center = (_low + _high) / 2;
    int band_width = (_high - _low);

    // create a 1-channel Butterworth Bandpass of 2,
    // with parameter smoothing and apply it to the audio data.
    filter = new Dsp::SmoothedFilterDesign
            <Dsp::Butterworth::Design::BandPass <2>, 1, Dsp::DirectFormII> (_fft_len);
    Dsp::Params params;
    params[0] = _sampling_rate;  // sampling rate
    params[1] = _order;  // order
    params[2] = center;  // center frequency
    params[3] = band_width;  // band width
    filter->setParams(params);
}

/**
 * Butterworth Bandpass Filter 수행
 *
 * @param data_out Bandpass된 출력 신호
 * @param data 입력 신호
 * @param _length 입력 신호의 길이
 */
void STFT::bandpass(double* data_out, double* data, int _length)
{
    double* data_tmp[1];  // 1 channels
    data_tmp[0] = new double[_length];
    memcpy(data_tmp[0], data, _length*sizeof(double));  // copy for pre-processing

    filter->process(_length, data_tmp);  // bandpass

    memcpy(data_out, data_tmp[0], _length*sizeof(double));  // copy for output
                                                              // data_out should be initialized with size of _length
}

/**
 * Bandpass Filter 수행 여부 설정
 *
 * @param _pre_processing Bandpass Filter 수행 여부
 */
void STFT::set_pre_processing(bool _pre_processing)
{
    pre_processing = _pre_processing;
}
