#include "SMU_use_opencv.h"			//opencv를 이용한 영상처리에 도움주는 헤더파일
#include "SMU_PupilDiameter.h"		//동공으로 생체정보 추론 및 감성 인식

namespace SMUPDFuncs
{
	class SMUroi_classFuncs
	{
	public:
		//header function
		void proc_videoMat(Mat origMat, bool gray_showon, bool binary_showon, bool pupil_tracking_showon);		//영상처리에 주요 함수
		void binary_ROI(Mat colorMat, Mat& grayMat, Mat& binaryMat, int checkpoint);							//이진화 함수
		void contour_ROI(Mat& frame, Mat& binaryMat, int checkpoint);											//윤곽선(네모, 동그라미) 및 동공 지름 얻는 함수 
		void MatToRGB(Mat& src, int checkpoint);
		void AutoThreshold(double m, int max, int checkpoint);

		//생체 정보 및 개인내 개인간 사회감성
		int getPupilDiameter();				//Pupil Diameter 가져오는 함수
		double getHR();						//HR 가져오는 함수														
		int getBPM();						//BPM 가져오는 함수	
		int get_w180_SE_Competition_P();	//개인내 경쟁협력 점수(%)
		int get_w180_SE_Focus_P();			//개인내 몰입도 점수(%)
		int get_w60_SE_intimacy_P();		//개인간 친밀도 점수(%)
		int get_w60_SE_empathy_P();			//개인간 공감도 점수(%)
		int get_w120_SE_Competition_P();		//개인간 친밀도 점수(%)
		int get_w120_SE_High_Focus_P();			//개인간 공감도 점수(%)	
	};
}
