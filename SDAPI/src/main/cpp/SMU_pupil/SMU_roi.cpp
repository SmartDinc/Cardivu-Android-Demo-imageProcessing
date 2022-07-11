#include "SMU_roi.h"

#define CHECKPOINT 1	//영상처리 적용 여부
#define MAX_SIZE 30000	//찾을 동공의 최대 크기(pixel) 설정, 주로 사용하는 범위: [10000 - 40000]
#define MIN_SIZE 1500	//찾을 동공의 최소 크기(pixel) 설정, 주로 사용하는 범위: [1500 - 3000]
#define THRESH 223		//이진화할 때 사용할 기준(기준으로 이상은 흰색, 미만은 검정색)

using namespace SMUPDFuncs;

namespace SMUPDFuncs
{
	SMUPDFuncs::SMUPupilDiameter_class *smuPD;

	//Rect_Mat
	Rect rectL;			//동공 ROI(관심 영역)에 최소 크기에 대한 정보(x,y,width,height) 변수
	Rect pupilL;		//contour_ROI함수에서 가져온 동공 트래킹 박스 크기 정보(동공 근처 빨간 네모 정보)
	Mat grayroiL, binaryroiL;

	int auto_threshold_L = 0;		//현재 threshold 
	int pre_auto_threshold_L = 0;	//이전 threshold

	bool first_check = true;		//처음 체크
	bool this_gray_showon = true;
	bool this_binary_showon = true;
	bool this_pupil_tracking_showon = true;

	char str[1024] = { 0 };
	int c_cnt;

	int frame_Cnt = 0;
	int pupil_diameter = 0;
	double HR_rPD_rule = 0;
	double buff_HR_rPD_rule = 0;
	//개인내
	int intra_Competition_P_intra_Focus_P = 0;
	int intra_Competition_percent = 0;
	int intra_Focus_percent = 0;
	//개인간
	int intimacy_P_empathy_P = 0;
	int intimacy_percent = 0;
	int empathy_percent = 0;
	int inter_Competition_P_inter_High_Focus_P = 0;
	int inter_Competition_percen = 0;
	int inter_High_Focus_percen = 0;

	//영상 처리
	void SMUroi_classFuncs::proc_videoMat(Mat origMat, bool gray_showon, bool binary_showon, bool pupil_tracking_showon)
	{
		Mat colorMat = origMat.clone();		//원본 영상(origMat)으로 복제(clone)한 후 colorMat에 저장(변수 초기화)
		Mat	grayMat = origMat.clone();		//원본 영상(origMat)으로 복제(clone)한 후 grayMat에 저장(변수 초기화)
		Mat	binaryMat = origMat.clone();	//원본 영상(origMat)으로 복제(clone)한 후 binaryMat에 저장(변수 초기화)
		//Mat	grayMat;		
		//Mat	binaryMat;	

		//imshow로 영상 출력 여부
		this_gray_showon = gray_showon;
		this_binary_showon = binary_showon;
		this_pupil_tracking_showon = pupil_tracking_showon;

		first_check = true;
		frame_Cnt++;					//현재까지의 프레임 갯수를 1증가 시킴
		if (frame_Cnt > 0)
		{
			if (first_check == true
				|| (pupilL.x - (pupilL.width / 30)) < 0 || (pupilL.y - (pupilL.width / 30)) < 0 || (pupilL.x + (pupilL.width / 30)) < 0 || (pupilL.y + (pupilL.width / 30)) < 0
				|| (pupilL.x + (pupilL.width + (pupilL.width / 30))) > rectL.width || (pupilL.y + (pupilL.width + (pupilL.width / 30))) > rectL.height)
			{	//동공 ROI(광심 영역)의 크기가 적절한 경우

				first_check = false;	//처음 프레임을 체크하는 변수

				binary_ROI(colorMat, grayroiL, binaryroiL, CHECKPOINT);	//이진화 함수
				contour_ROI(colorMat, binaryroiL, CHECKPOINT);			//윤곽선(네모, 동그라미) 및 동공 지름 얻는 함수 

				//binary_ROI(colorMat, grayroiL, binaryroiL, CHECKPOINT);	//이진화 함수
				//contour_ROI(colorMat, binaryroiL, CHECKPOINT);			//윤곽선(네모, 동그라미) 및 동공 지름 얻는 함수 
			}
			else
			{	//동공 ROI(광심 영역)의 크기가 적절하지 않을 경우

				Mat roiL_on = colorMat(rectL);	//원본 영상을 rectL(동공 ROI크기)로 잘라낸 후 roiL_on에 저장
				//Mat grayroiL_on, binaryroiL_on;

				binary_ROI(roiL_on, grayroiL, binaryroiL, CHECKPOINT);	//이진화 함수
				contour_ROI(roiL_on, binaryroiL, CHECKPOINT);				//윤곽선(네모, 동그라미) 및 동공 지름 얻는 함수 

				//binary_ROI(roiL_on, grayroiL_on, binaryroiL_on, CHECKPOINT);	//이진화 함수
				//contour_ROI(roiL_on, binaryroiL_on, CHECKPOINT);				//윤곽선(네모, 동그라미) 및 동공 지름 얻는 함수 
			}
			if (this_gray_showon){
				imshow("grayMatView", grayMat);		//grayMat - imshow
			}
			if (this_binary_showon){
				imshow("binaryMatView", binaryMat);	//binaryMat - imshow
			}
		}
	}

	void SMUroi_classFuncs::binary_ROI(Mat colorMat, Mat& grayMat, Mat& binaryMat, int checkpoint)
	{
		cvtColor(~colorMat, grayMat, CV_BGR2GRAY);	//RGB프레임(colorMat)을 반전(~)한 후 회색조(광도만 표시,CV_BGR2GRAY)로 변화(cvtColor)후 grayMat변수에 저장
		MatToRGB(grayMat, checkpoint);				//#define THRESH를 값을 조절하여 auto_threshold_L 변수에 넣어주는 함수

		if (checkpoint == 1)
		{
			threshold(grayMat, binaryMat, auto_threshold_L, 255, THRESH_BINARY); //이진화 함수 [threshold(이진화할 프레임, 저장할 프레임, 이진화 기준값 변수, 최댓값(명도의 최댓값은 255), 이진화 종류)]

			if (pre_auto_threshold_L != auto_threshold_L) {	//Threshold(이진화의 기준)가 변경될 경우
				//cout << "Threshold(L) 갱신 = " << auto_threshold_L << endl;
				pre_auto_threshold_L = auto_threshold_L;
			}
		}
	}

	void SMUroi_classFuncs::contour_ROI(Mat& frame, Mat& binaryMat, int checkpoint)
	{	//이진화(binaryMat) 영상으로 윤곽선을 구하고 원본(frame) 영상을 복제한 영상에 동공 주위에 ROI영역(네모, 원) 그려줌
		int points = 50;
		RNG rng(12345);

		Mat cMat = frame.clone();

		// Find all contours
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		findContours(binaryMat.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);	//이진화된 영상(binaryMat)에서 윤곽선을 찾아 contours변수에 저장

		vector<RotatedRect> minRect(contours.size());
		vector<RotatedRect> minEllipse(contours.size());

		for (int i = 0; i < contours.size(); i++)
		{
			minRect[i] = minAreaRect(Mat(contours[i]));

			if (contours[i].size() > points)
			{
				minEllipse[i] = fitEllipse(Mat(contours[i]));
			}
		}

		// Fill holes in each contour
		drawContours(binaryMat, contours, -1, CV_RGB(255, 255, 255), -1);


		// Fill holes in each contour
		for (int i = 0; i < contours.size(); i++)
		{
			double area = contourArea(contours[i]);
			Rect rect = boundingRect(contours[i]);
			double radius = rect.width / 2.0;

			if (area <= MAX_SIZE && area >= MIN_SIZE && abs(1 - (area / (CV_PI * (radius*radius)))) <= 0.38
				&& (minRect[i].size.width*1.5 > minRect[i].size.height) && (minRect[i].size.width < minRect[i].size.height*1.5)
				&& (minRect[i].center.x < cMat.cols*0.8) && (minRect[i].center.y < cMat.rows*0.8)
				)
			{
				if (checkpoint == 1)
				{
					pupilL = rect;
				}

				// contour
				drawContours(cMat, contours, i, CV_RGB(255, 0, 0), 1, 8, vector<Vec4i>(), 0, Point());	//윤곽선을 합쳐서 이은거 (자잘자잘한 선)

				// ellipse
				ellipse(cMat, minEllipse[i], CV_RGB(255, 0, 0), 2, 8);	//윤곽선 이은거를 타원으로 변형함

				// rotated rectangle
				Point2f rect_points[4];
				minRect[i].points(rect_points);//width				
				for (int j = 0; j < 4; j++)
					line(cMat, rect_points[j], rect_points[(j + 1) % 4], CV_RGB(255, 0, 0), 1, 8);

				Point bl = Point(rect.x - 130, rect.y + rect.height + 30);
				stringstream ss;
				ss << "frame_Cnt : " << frame_Cnt << ",  r= " << (int)radius * 2 << "   x = " << (int)rect.x + (int)radius << "   y = " << ((int)rect.y + (int)radius);
				cv::putText(cMat, ss.str(), bl, FONT_HERSHEY_PLAIN, 1.5, CV_RGB(200, 200, 0), 2);	//frame_Cnt, x, y을 화면에 넣음

				//cv::namedWindow("Contours", CV_WINDOW_AUTOSIZE);
				if (this_pupil_tracking_showon){
					cv::imshow("trackingView", cMat);
				}

				pupil_diameter = rect.width;		//동공 지름: pupil_diameter
				//printf("%d \n", pupil_diameter);

			}
		}

		//생체정보 HR or BPM
		HR_rPD_rule = smuPD->PupilDiameter_HR(pupil_diameter);	//생체정보 추론
		buff_HR_rPD_rule = HR_rPD_rule;

		//사회감성 개인내 - 경쟁협력 & 몰입도 => intra_Focus_percent; intra_Competition_percent;
		intra_Competition_P_intra_Focus_P = (int)smuPD->PupilDiameter_SE_w180(pupil_diameter);	//사회감성 인식
		intra_Focus_percent = intra_Competition_P_intra_Focus_P % 1000;
		intra_Competition_percent = (intra_Competition_P_intra_Focus_P - intra_Focus_percent) / 1000;

		//사회감성 개인간(다중사용자) - 친밀도 & 공감도 => intimacy_P; empathy_P
		intimacy_P_empathy_P = (int)smuPD->PupilDiameter_SE_w60(pupil_diameter);	//사회감성 인식
		intimacy_percent = intimacy_P_empathy_P % 1000;
		empathy_percent = (intimacy_P_empathy_P - intimacy_percent) / 1000;

		//사회감성 개인간(다중사용자) - 경쟁협력 & 몰입도 => inter_Competition_P; inter_High_Focus_P
		inter_Competition_P_inter_High_Focus_P = (int)smuPD->PupilDiameter_SE_w120(pupil_diameter);	//사회감성 인식
		inter_High_Focus_percen = inter_Competition_P_inter_High_Focus_P % 1000;
		inter_Competition_percen = (inter_Competition_P_inter_High_Focus_P - inter_High_Focus_percen) / 1000;
	}

	int SMUroi_classFuncs::getPupilDiameter(){
		return pupil_diameter;
	}

	double SMUroi_classFuncs::getHR(){
		return buff_HR_rPD_rule;
	}

	int SMUroi_classFuncs::getBPM(){
		int BPM = (int)(((60 / buff_HR_rPD_rule) > 0) ? (60 / buff_HR_rPD_rule) : 0);
		//예외 처리
		if (_isnan(BPM) || (!_finite(BPM)) || buff_HR_rPD_rule == 0){//_finitef 
			BPM = 0;
		}
		return BPM;
	}

	int SMUroi_classFuncs::get_w180_SE_Competition_P(){
		return intra_Competition_percent;
	}

	int SMUroi_classFuncs::get_w180_SE_Focus_P(){
		return intra_Focus_percent;
	}

	int SMUroi_classFuncs::get_w60_SE_intimacy_P(){
		return intimacy_percent;
	}

	int SMUroi_classFuncs::get_w60_SE_empathy_P(){
		return empathy_percent;
	}

	int SMUroi_classFuncs::get_w120_SE_Competition_P(){
		return inter_Competition_percen;
	}

	int SMUroi_classFuncs::get_w120_SE_High_Focus_P(){
		return inter_High_Focus_percen;
	}

	void SMUroi_classFuncs::MatToRGB(Mat& src, int checkpoint)//MatToRGB: THRESH를 값을 조절하여 auto_threshold_L 변수에 넣어주는 함수
	{
		IplImage* frame;
		unsigned char *data;
		int R = 0, G = 0, B = 0, mx = 0, mn = 0, max = 0, min = 255;
		double r = 0, g = 0, b = 0, smx = 0, smn = 0, c_mean = 0;
		int nIdx;

		frame = &IplImage(src);
		data = (unsigned char*)frame->imageData;

		for (int y = 0; y < (frame->height) - 0; y++)
		{
			for (int x = 0; x < (frame->width) - 0; x++)
			{
				nIdx = y * frame->widthStep + x * frame->nChannels;

				R = data[nIdx + 2];
				G = data[nIdx + 1];
				B = data[nIdx + 0];

				//#define MATH_MIN3(x,y,z)		( (y) <= (z) ? ((x) <= (y) ? (x) : (y)) : ((x) <= (z) ? (x) : (z)) )
				//#define MATH_MAX3(x,y,z)		( (y) >= (z) ? ((x) >= (y) ? (x) : (y)) : ((x) >= (z) ? (x) : (z)) )
				//mx = MATH_MAX3(B, G, R);
				//mn = MATH_MIN3(B, G, R);
				mx = ((G) <= (R) ? ((B) <= (G) ? (B) : (G)) : ((B) <= (R) ? (B) : (R)));
				mn = ((G) >= (R) ? ((B) >= (G) ? (B) : (G)) : ((B) >= (R) ? (B) : (R)));

				if (1)
				{
					if (max < mx)
					{
						max = mx;
					}
					smx = smx + mx;

					if (min > mn)
					{
						min = mn;
					}
					smn = smn + mn;
				}
				r = r + R;
				g = g + G;
				b = b + B;
			}
		}

		c_mean = (smx / (double)(frame->height*frame->width));

		AutoThreshold(c_mean, max, checkpoint);
	}

	void SMUroi_classFuncs::AutoThreshold(double m, int max, int checkpoint)//AutoThreshold: MatToRGB 내부에서 사용되어 THRESH를 값을 조절하여 auto_threshold_L 변수에 넣어주는 함수
	{
		if (checkpoint == 1){
			if (THRESH == 0)
				auto_threshold_L = int((max - 35) + (m / 10));
			else if (THRESH != 0)
				auto_threshold_L = THRESH;
			else
			{
				if (THRESH == 0)
					auto_threshold_L = THRESH;
				else if (THRESH != 0)
					auto_threshold_L = int((max - 35) + (m / 10));
			}
		}
	}
}
