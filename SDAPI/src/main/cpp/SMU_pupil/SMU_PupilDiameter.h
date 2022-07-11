#include "SMU_signal.h"

#include <iostream>
using namespace std;

SMUPDFuncs::SMUSignal_class *hrv_smusig;

namespace SMUPDFuncs
{
	class SMUPupilDiameter_class
	{
	public:
		double PupilDiameter_HR(double radius);			//생체정보
		void setrPD_SD(double rPD_SD);	//rPD_SD
		double getrPD_SD();				//rPD_SD
		void setrPD_HR(double rPD_HR);
		double getrPD_HR();
		double PupilDiameter_SE_w180(double radius);	//사회감성
		double PupilDiameter_SE_w60(double radius);		//사회감성
		double PupilDiameter_SE_w120(double radius);	//사회감성
	};
}
