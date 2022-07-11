#include <cstdlib>
#include "SMU_real_double_fft_mixed.h"


namespace SMUPDFuncs
{
	class RealDoubleFFT : RealDoubleFFTMixed
	{
	public:
		RealDoubleFFT(int _n);
		~RealDoubleFFT();

		void ft(double* _x, int _length);
		void fft_to_amp(double* data_out, double* data, int _length);
	};
}
