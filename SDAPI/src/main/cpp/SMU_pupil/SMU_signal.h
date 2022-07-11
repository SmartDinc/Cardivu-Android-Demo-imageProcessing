#include "SMU_real_double_fft.h"
using namespace SMUPDFuncs;
#include "..//DSPFilters//dsp.h"
using namespace Dsp;

#include <stdlib.h> 
#include <math.h> 
#include <iostream>
using namespace std;

namespace SMUPDFuncs
{
	class SMUSignal_class
	{
	public:
		double* fft(double* data, int length, int time);
		void bandpass(double* inputData, int sampling_rate, double low, double high, int order, double* outputData, int length);
		double mean(double* array, int size);
		double standardDeviation(double* array, int size, int option);//랩뷰 모집단 - option :1, 엑셀 : STDEV.P => 0 , STDEV.S => 1

		//correlation algorithm
		double corcoef(double *x, double *y, int n);
		double cosine_similarity(double *A, double *B, unsigned int Vector_Length);
	};
}
