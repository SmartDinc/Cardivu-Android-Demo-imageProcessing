#include "SMU_signal.h"

namespace SMUPDFuncs
{
	/**
	* Fast Fourier Transform
	*
	* @param data 입력 신호
	* @param length 입력 신호의 길이
	* @param time 입력 신호의 시간
	*/
	double* SMUSignal_class::fft(double* data, int length, int time)
	{
		// Calculate df
		double fs = (double)length / (double)time;
		double df = fs / length;

		// FFT
		int ps_len = length / 2;
		double* spectrum_amp_in = new double[length];
		double* spectrum_amp_out = new double[ps_len];
		RealDoubleFFT* spectrum_amp_fft = new RealDoubleFFT(length);
		memcpy(spectrum_amp_in, data, length * sizeof(double));
		spectrum_amp_fft->ft(spectrum_amp_in, length);
		spectrum_amp_fft->fft_to_amp(spectrum_amp_out, spectrum_amp_in, length);

		// Release
		//delete spectrum_amp_fft;
		//delete spectrum_amp_in;

		return spectrum_amp_out;
	}

	/**
	* Butterworth Bandpass Filter
	*
	* @param inputData 입력 데이터 배열
	* @param sampling_rate 입력 신호의 Sampling Rate (Hz)
	* @param low 저주파 통과 대역
	* @param high 고주파 통과 대역
	* @param order 필터의 차수
	* @param outputData 처리한 데이터 배열
	* @param length 입력 신호의 길이 - 두 배열의 길이
	*/
	void SMUSignal_class::bandpass(double* inputData, int sampling_rate, double low, double high, int order, double* outputData, int length)
	{
		// calculate parameters for bandpass
		double center = (low + high) / 2;
		double band_width = (high - low);
		Params params;
		// create a 1-channel Butterworth Bandpass of 2,
		// with parameter smoothing and apply it to the audio data.
		Filter* filter = new SmoothedFilterDesign
			<Butterworth::Design::BandPass <2>, 1, DirectFormII>(length);  //Butterworth::Design::BandPass <order>로 맞춰줘야함
		//Params params;
		params[0] = sampling_rate;  // sampling rate
		params[1] = order;  // order
		params[2] = center;  // center frequency
		params[3] = band_width;  // band width
		filter->setParams(params);

		memset(outputData, 1, length * sizeof(double));

		double* data_tmp[1];  // 1 channels
		data_tmp[0] = new double[length];
		memcpy(data_tmp[0], inputData, length * sizeof(double));  // copy for pre-processing

		filter->process(length, data_tmp);  // bandpass

		memcpy(outputData, data_tmp[0], length * sizeof(double));
	}

	double SMUSignal_class::mean(double* array, int size) {	// 산술 평균 구하기
		double sum = 0.0;

		for (int i = 0; i < size; i++)
			sum += array[i];

		return sum / size;
	}

	double SMUSignal_class::standardDeviation(double* array, int size, int option) {	// 표준 편차 계산 함수
		if (size < 2) return sqrt(-1.0);

		double sum = 0.0;
		double sd = 0.0;
		double diff;
		double meanValue = mean(array, size);

		for (int i = 0; i < size; i++) {
			diff = array[i] - meanValue;
			sum += diff * diff;
		}
		sd = sqrt(sum / (size - option));

		return sd;
	}

	double SMUSignal_class::corcoef(double *x, double *y, int n)
	{
		double sx, sy, sxy;
		double sx2, sy2, r;
		int i;
		sx = sy = sxy = 0.0;
		sx2 = sy2 = 0.0;
		for (i = 0; i < n; i++) {
			sx += x[i];
			sy += y[i];
			sxy += x[i] * y[i];
			sx2 += x[i] * x[i];
			sy2 += y[i] * y[i];
		}
		r = (n*sxy - sx * sy) / (sqrt(n*sx2 - sx * sx)*sqrt(n*sy2 - sy * sy));

		//예외 처리
		if (_isnan(r) || (!_finite(r))) {
			r = 0;
		}
		if (isinf(r)) {
			r = 1;
		}
		return r;
	}

	double SMUSignal_class::cosine_similarity(double *A, double *B, unsigned int Vector_Length)
	{
		double dot = 0.0, denom_a = 0.0, denom_b = 0.0;
		for (unsigned int i = 0u; i < Vector_Length; ++i) {
			dot += A[i] * B[i];
			denom_a += A[i] * A[i];
			denom_b += B[i] * B[i];
		}
		return dot / (sqrt(denom_a) * sqrt(denom_b));
	}
}
