#include "SMU_PupilDiameter.h"

namespace SMUPDFuncs
{
	SMUPDFuncs::SMUSignal_class *smusig;
	///////////////////////생체 정보 HR, BPM

	int HR_frame_Cnt = 0;
	const int sampling_rate = 30;  // Hz		30
	int HR_rPD_Cnt = 0;
	double HR_rPD_SD = 0;
	double set_rPD_HR = 0;

	//Data scanning
	double rawdataPD[30 * 30] = { 0 };
	double copy_rawdataPD[30 * 30] = { 0 };
	double rawdataZeroPD[30 * 30] = { 0 };
	int  rawCount = 0;

	//1Hz resampling
	double PD_Avg = 0;
	double PD_Array[30] = { 0 };
	double PD_Array_Sum = 0;

	//PD
	double PD_SD = 0;
	double PD_BPF_Avg = 0;
	double PD_BPF_SD = 0;
	double PD_BPFV_Avg = 0;
	double PD_BPFV_SD = 0;
	double PD_BPF_Array[30] = { 0 };
	double PD_BPFV_Array[29] = { 0 };

	//PDV
	double PDV_Avg = 0;
	double PDV_SD = 0;
	double PDV_BPF_Avg = 0;
	double PDV_BPF_SD = 0;
	double PDV_BPFV_Avg = 0;
	double PDV_BPFV_SD = 0;
	double PDV_Array[29] = { 0 };
	double PDV_BPF_Array[29] = { 0 };
	double PDV_BPFV_Array[28] = { 0 };

	// rPD
	double rPD_Avg = 0;
	double rPD_SD = 0;
	double rPD_BPF_Avg = 0;
	double rPD_BPF_SD = 0;
	double rPD_BPFV_Avg = 0;
	double rPD_BPFV_SD = 0;
	double rPD_Array[30] = { 0 };
	double rPD_BPF_Array[30] = { 0 };
	double rPD_BPFV_Array[29] = { 0 };

	//rPDV
	double rPDV_Avg = 0;
	double rPDV_SD = 0;
	double rPDV_BPF_SD = 0;
	double rPDV_BPF_Avg = 0;
	double rPDV_BPFV_Avg = 0;
	double rPDV_BPFV_SD = 0;
	double rPDV_Array[29] = { 0 };
	double rPDV_BPF_Array[29] = { 0 };
	double rPDV_BPFV_Array[28] = { 0 };

	double pre_HR_rPD_rule = 0;
	double pre_Discriminant_Score = 0;
	bool first_Dis = true;
	bool first_HR = true;

	///////////////////////w180 개인내 경쟁협력 몰입도
	int w180_frame_Cnt = 0;
	int w180_HR_rPD_Cnt = 0;

	//Data scanning
	double w180_rawdataPD[30 * 180] = { 0 };
	double w180_copy_rawdataPD[30 * 180] = { 0 };
	double w180_rawdataZeroPD[30 * 180] = { 0 };
	int w180_rawCount = 0;

	//1Hz resampling
	double w180_PD_Avg = 0;
	double w180_PD_Array[180] = { 0 };
	double w180_PD_Array_Sum = 0;

	//w180_PD
	double w180_PD_SD = 0;
	double w180_PD_BPF_Avg = 0;
	double w180_PD_BPF_SD = 0;
	double w180_PD_BPFV_Avg = 0;
	double w180_PD_BPFV_SD = 0;
	double w180_PD_BPF_Array[180] = { 0 };
	double w180_PD_BPFV_Array[179] = { 0 };

	//w180_PDV
	double w180_PDV_Avg = 0;
	double w180_PDV_SD = 0;
	double w180_PDV_BPF_Avg = 0;
	double w180_PDV_BPF_SD = 0;
	double w180_PDV_BPFV_Avg = 0;
	double w180_PDV_BPFV_SD = 0;
	double w180_PDV_Array[179] = { 0 };
	double w180_PDV_BPF_Array[179] = { 0 };
	double w180_PDV_BPFV_Array[178] = { 0 };

	// rw180_PD
	double w180_rPD_Avg = 0;
	double w180_rPD_SD = 0;
	double w180_rPD_BPF_Avg = 0;
	double w180_rPD_BPF_SD = 0;
	double w180_rPD_BPFV_Avg = 0;
	double w180_rPD_BPFV_SD = 0;
	double w180_rPD_Array[180] = { 0 };
	double w180_rPD_BPF_Array[180] = { 0 };
	double w180_rPD_BPFV_Array[179] = { 0 };

	// w180_rPDV
	double w180_rPDV_Avg = 0;
	double w180_rPDV_SD = 0;
	double w180_rPDV_BPF_SD = 0;
	double w180_rPDV_BPF_Avg = 0;
	double w180_rPDV_BPFV_Avg = 0;
	double w180_rPDV_BPFV_SD = 0;
	double w180_rPDV_Array[179] = { 0 };
	double w180_rPDV_BPF_Array[179] = { 0 };
	double w180_rPDV_BPFV_Array[178] = { 0 };

	double NN_PSV_Array[180] = { 0 };
	double NN_FFT[(180) / 2] = { 0 };
	double VLF_NN_FFT[(180) / 2] = { 0 };
	double HF_NN_FFT[(180) / 2] = { 0 };
	double NN_BPF[180] = { 0 };
	double VLF_NN_BPF[180] = { 0 };
	double HF_NN_BPF[180] = { 0 };
	double squares_NN[180] = { 0 };

	//intra_Competition
	double intra_Competition_P = 0;
	double intra_Focus_P = 0;

	double intra_Competition_50 = 0.7;
	double intra_Competition_100 = intra_Competition_50 + (0.95*1.5);	//2.475
	double intra_Competition_0 = intra_Competition_50 - (0.4 * 3);	//0.45

	//intra_Focus
	double intra_Focus_50 = 1;
	double intra_Focus_100 = intra_Focus_50 + (3 * 1.5);	//7.5
	double intra_Focus_0 = intra_Focus_50 - (2.5 * 5);	//-2.25

	///////////////////////w30 개인간 몰입도 공감도
	double SE_w60_SDNN_Array[30] = { 0 };
	double SE_w60_other_SDNN_Array[30] = { 0 };
	int SE_w60_frame_Cnt = 0;
	int SE_w60_SDNN_Array_Cnt = 0;

	///////////////////////w120 개인간 몰입도 공감도
	///////////////////////SE_w120
	int SE_w120_frame_Cnt = 0;
	int SE_w120_rPD_Cnt = 0;

	//Data scanning
	double SE_w120_rawdataPD[30 * 120] = { 0 };
	double SE_w120_copy_rawdataPD[30 * 120] = { 0 };
	double SE_w120_rawdataZeroPD[30 * 120] = { 0 };
	int  SE_w120_rawCount = 0;

	//1Hz resampling
	double SE_w120_PD_Avg = 0;
	double SE_w120_PD_Array[120] = { 0 };
	double SE_w120_PD_Array_Sum = 0;

	//PD
	double SE_w120_PD_SD = 0;
	double SE_w120_PD_BPF_Avg = 0;
	double SE_w120_PD_BPF_SD = 0;
	double SE_w120_PD_BPFV_Avg = 0;
	double SE_w120_PD_BPFV_SD = 0;
	double SE_w120_PD_BPF_Array[120] = { 0 };
	double SE_w120_PD_BPFV_Array[119] = { 0 };

	//PDV
	double SE_w120_PDV_Avg = 0;
	double SE_w120_PDV_SD = 0;
	double SE_w120_PDV_BPF_Avg = 0;
	double SE_w120_PDV_BPF_SD = 0;
	double SE_w120_PDV_BPFV_Avg = 0;
	double SE_w120_PDV_BPFV_SD = 0;
	double SE_w120_PDV_Array[119] = { 0 };
	double SE_w120_PDV_BPF_Array[119] = { 0 };
	double SE_w120_PDV_BPFV_Array[118] = { 0 };

	// rPD
	double SE_w120_rPD_Avg = 0;
	double SE_w120_rPD_SD = 0;
	double SE_w120_rPD_BPF_Avg = 0;
	double SE_w120_rPD_BPF_SD = 0;
	double SE_w120_rPD_BPFV_Avg = 0;
	double SE_w120_rPD_BPFV_SD = 0;
	double SE_w120_rPD_Array[120] = { 0 };
	double SE_w120_rPD_BPF_Array[120] = { 0 };
	double SE_w120_rPD_BPFV_Array[119] = { 0 };

	// rPDV
	double SE_w120_rPDV_Avg = 0;
	double SE_w120_rPDV_SD = 0;
	double SE_w120_rPDV_BPF_SD = 0;
	double SE_w120_rPDV_BPF_Avg = 0;
	double SE_w120_rPDV_BPFV_Avg = 0;
	double SE_w120_rPDV_BPFV_SD = 0;
	double SE_w120_rPDV_Array[119] = { 0 };
	double SE_w120_rPDV_BPF_Array[119] = { 0 };
	double SE_w120_rPDV_BPFV_Array[118] = { 0 };

	//FFT
	double SE_w120_NN_PSV_Array[120] = { 0 };
	double SE_w120_NN_FFT[(120) / 2] = { 0 };
	double SE_w120_LF_NN_FFT[(120) / 2] = { 0 };
	double SE_w120_VLF_NN_FFT[(120) / 2] = { 0 };
	double SE_w120_HF_NN_FFT[(120) / 2] = { 0 };
	double SE_w120_TP_NN_FFT[(120) / 2] = { 0 };
	double SE_w120_NN_BPF[120] = { 0 };
	double SE_w120_LF_NN_BPF[120] = { 0 };
	double SE_w120_VLF_NN_BPF[120] = { 0 };
	double SE_w120_HF_NN_BPF[120] = { 0 };
	double SE_w120_TP_NN_BPF[120] = { 0 };
	double SE_w120_squares_NN[120] = { 0 };

	//Social Emotion: 개인간(interindividual) - 경쟁 협력(Competition), 몰입도(Focus)
	int interindividua_Cnt = 0;
	//사용자
	double SE_w120_BPM_Array[30] = { 0 };
	double SE_w120_RRI_Array[30] = { 0 };
	double SE_w120_SDNN_Array[30] = { 0 };
	double SE_w120_rMSSD_Array[30] = { 0 };
	double SE_w120_sum_VLF_NN_BPF_Array[30] = { 0 };
	double SE_w120_sum_LF_NN_BPF_Array[30] = { 0 };
	double SE_w120_sum_HF_NN_BPF_Array[30] = { 0 };
	double SE_w120_VLF_p_Array[30] = { 0 };
	double SE_w120_LF_p_Array[30] = { 0 };
	double SE_w120_HF_p_Array[30] = { 0 };
	double SE_w120_VLF_HF_Array[30] = { 0 };
	double SE_w120_LF_HF_Array[30] = { 0 };
	double SE_w120_log_LF_Array[30] = { 0 };
	double SE_w120_log_HF_Array[30] = { 0 };
	double SE_w120_log_LF_log_HF_Array[30] = { 0 };
	double SE_w120_TP_DPF_Array[30] = { 0 };
	double SE_w120_HRC_Array[30] = { 0 };

	//상대방
	double other_SE_w120_BPM_Array[30] = { 0 };
	double other_SE_w120_RRI_Array[30] = { 0 };
	double other_SE_w120_SDNN_Array[30] = { 0 };
	double other_SE_w120_rMSSD_Array[30] = { 0 };
	double other_SE_w120_sum_VLF_NN_BPF_Array[30] = { 0 };
	double other_SE_w120_sum_LF_NN_BPF_Array[30] = { 0 };
	double other_SE_w120_sum_HF_NN_BPF_Array[30] = { 0 };
	double other_SE_w120_VLF_p_Array[30] = { 0 };
	double other_SE_w120_LF_p_Array[30] = { 0 };
	double other_SE_w120_HF_p_Array[30] = { 0 };
	double other_SE_w120_VLF_HF_Array[30] = { 0 };
	double other_SE_w120_LF_HF_Array[30] = { 0 };
	double other_SE_w120_log_LF_Array[30] = { 0 };
	double other_SE_w120_log_HF_Array[30] = { 0 };
	double other_SE_w120_log_LF_log_HF_Array[30] = { 0 };
	double other_SE_w120_TP_DPF_Array[30] = { 0 };
	double other_SE_w120_HRC_Array[30] = { 0 };

	//생체정보 - HR, BPM
	double SMUPupilDiameter_class::PupilDiameter_HR(double radius) {
		double HR_rPD_rule = 0;

		rawdataPD[rawCount] = (int)radius;
		rawCount++;
		if (HR_frame_Cnt % 30 == 0) {
			//cout << HR_frame_Cnt / 30 << "초" << endl;
		}

		//30초부터 1초씩~
		if (HR_frame_Cnt % 30 == 0 && HR_frame_Cnt >= (30 * 30)) {
			int rawdataZeroCnt = 0;
			double rawdataSum = 0, rawdataAvg = 0, rawdataPDSD = 0;

			//합계 => rawdataSum
			for (int rawNum = 0; rawNum < sampling_rate * 30; rawNum++) {
				rawdataSum += rawdataPD[rawNum];
				if (rawdataPD[rawNum] != 0) {
					rawdataZeroPD[rawdataZeroCnt] = rawdataPD[rawNum];
					rawdataZeroCnt++;
				}
			}

			//표준 편차
			rawdataAvg = (rawdataSum / rawdataZeroCnt);
			rawdataPDSD = smusig->standardDeviation(rawdataZeroPD, rawdataZeroCnt, 1);

			//2.5sigma - 이상치 결정
			memcpy(copy_rawdataPD, rawdataPD, sizeof(rawdataPD));
			double rawdataPDSDSum = 0, rawdataPDSDCnt = 0, rawdataPDSDAvg = 0;
			for (int rawNum = 0; rawNum < sampling_rate * 30; rawNum++) {
				if ((copy_rawdataPD[rawNum] > (rawdataAvg + rawdataPDSD * 2.5)) || (copy_rawdataPD[rawNum] < (rawdataAvg - rawdataPDSD * 2.5))) {
					copy_rawdataPD[rawNum] = 0;
				}
			}

			//1Hz resampling && PD_Avg
			PD_Array_Sum = 0;
			for (int for_PD_Avg = 0; for_PD_Avg < 30; for_PD_Avg++) {
				double Sum_1Hz = 0;
				int Cnt_1Hz = 0;

				for (int for_Raw_1Hz = for_PD_Avg * 30; for_Raw_1Hz < (for_PD_Avg + 1) * 30; for_Raw_1Hz++) {
					if (copy_rawdataPD[for_Raw_1Hz] != 0) {
						Sum_1Hz += copy_rawdataPD[for_Raw_1Hz];
						Cnt_1Hz++;
					}
				}
				if (Cnt_1Hz != 0) {
					PD_Array[for_PD_Avg] = Sum_1Hz / Cnt_1Hz;
				}
				else {
					PD_Array[for_PD_Avg] = 0;
				}
				PD_Array_Sum += PD_Array[for_PD_Avg];

			}
			PD_Avg = PD_Array_Sum / 30;	//PD_Avg

			//PDV_Array
			for (int for_PDV_Array = 0; for_PDV_Array < 29; for_PDV_Array++) {
				PDV_Array[for_PDV_Array] = abs(PD_Array[for_PDV_Array] - PD_Array[for_PDV_Array + 1]);
			}

			//rPD_Array
			double rPD_Array_MIN = 0;
			for (int for_rPD_Array = 0; for_rPD_Array < 30; for_rPD_Array++) {
				rPD_Array[for_rPD_Array] = (PD_Array[for_rPD_Array] - PD_Avg) / PD_Avg;

				if (rPD_Array_MIN > rPD_Array[for_rPD_Array]) {
					rPD_Array_MIN = rPD_Array[for_rPD_Array];
				}
			}
			rPD_Array_MIN = abs(rPD_Array_MIN);
			//|rPD|_Array
			for (int for_rPD_Array = 0; for_rPD_Array < 30; for_rPD_Array++) {
				rPD_Array[for_rPD_Array] = rPD_Array[for_rPD_Array] + rPD_Array_MIN;
			}
			//rPDV_Array
			for (int for_rPDV_Array = 0; for_rPDV_Array < 29; for_rPDV_Array++) {
				rPDV_Array[for_rPDV_Array] = abs(rPD_Array[for_rPDV_Array] - rPD_Array[for_rPDV_Array + 1]);
			}
			///////////////////////////////////////////////////////////PD
			//PD_SD
			PD_SD = 0;
			PD_SD = smusig->standardDeviation(PD_Array, 30, 1);

			//PD_BPF_Avg
			PD_BPF_Avg = 0;
			smusig->bandpass(PD_Array, 1, 0.0833, 0.2, 2, PD_BPF_Array, 30);		///PD_BPF
			PD_BPF_Avg = smusig->mean(PD_BPF_Array, 30);

			//PD_BPF_SD
			PD_BPF_SD = 0;
			PD_BPF_SD = smusig->standardDeviation(PD_BPF_Array, 30, 1);

			//PD_BPFV_Avg
			PD_BPFV_Avg = 0;
			for (int for_PD_BPFV = 0; for_PD_BPFV < 29; for_PD_BPFV++) {
				PD_BPFV_Array[for_PD_BPFV] = abs(PD_BPF_Array[for_PD_BPFV] - PD_BPF_Array[for_PD_BPFV + 1]);
			}
			PD_BPFV_Avg = smusig->mean(PD_BPFV_Array, 29);

			//PD_BPFV_SD
			PD_BPFV_SD = 0;
			PD_BPFV_SD = smusig->standardDeviation(PD_BPFV_Array, 29, 1);			//PD_BPFV_SD

			///////////////////////////////////////////////////////////PDV
			//PDV_Avg
			PDV_Avg = 0;
			PDV_Avg = smusig->mean(PDV_Array, 29);

			//PDV_SD
			PDV_SD = 0;
			PDV_SD = smusig->standardDeviation(PDV_Array, 29, 1);			//PDV_SD

			//PDV_BPF_Avg
			PDV_BPF_Avg = 0;
			smusig->bandpass(PDV_Array, 1, 0.0833, 0.2, 2, PDV_BPF_Array, 29);		///rPDV_BPF_Array
			PDV_BPF_Avg = smusig->mean(PDV_BPF_Array, 29);

			//PDV_BPF_SD
			PDV_BPF_SD = 0;
			PDV_BPF_SD = smusig->standardDeviation(PDV_BPF_Array, 29, 1);			//PDV_BPF_SD

			//PDV_BPFV_Avg
			PDV_BPFV_Avg = 0;
			for (int for_PDV_BPFV = 0; for_PDV_BPFV < 28; for_PDV_BPFV++) {
				PDV_BPFV_Array[for_PDV_BPFV] = abs(PDV_BPF_Array[for_PDV_BPFV] - PDV_BPF_Array[for_PDV_BPFV + 1]);
			}
			PDV_BPFV_Avg = smusig->mean(PDV_BPFV_Array, 28);

			//PDV_BPFV_SD
			PDV_BPFV_SD = 0;
			PDV_BPFV_SD = smusig->standardDeviation(PDV_BPFV_Array, 28, 1);		//PDV_BPFV_SD

			///////////////////////////////////////////////////////////rPD
			//rPD_Avg
			rPD_Avg = 0;
			rPD_Avg = smusig->mean(rPD_Array, 30);

			//rPD_SD
			rPD_SD = 0;
			rPD_SD = smusig->standardDeviation(rPD_Array, 30, 1);							//rPD_SD

			setrPD_SD(rPD_SD); //전역변수에 rPD_SD값 저장
			//getrPD_SD();
			//cout << "setrPD_SD(rPD_SD)" << rPD_SD << ", getrPD_SD(): " << getrPD_SD()<<endl;

			//rPD_BPF_Avg
			rPD_BPF_Avg = 0;
			smusig->bandpass(rPD_Array, 1, 0.0833, 0.2, 2, rPD_BPF_Array, 30);		///rPDV_BPF_Array
			rPD_BPF_Avg = smusig->mean(rPD_BPF_Array, 30);

			//rPD_BPF_SD
			rPD_BPF_SD = 0;
			rPD_BPF_SD = smusig->standardDeviation(rPD_BPF_Array, 30, 1);						//rPDV_SD

			//rPD_BPFV_Avg
			rPD_BPFV_Avg = 0;
			for (int for_rPD_BPFV = 0; for_rPD_BPFV < 29; for_rPD_BPFV++) {
				rPD_BPFV_Array[for_rPD_BPFV] = abs(rPD_BPF_Array[for_rPD_BPFV] - rPD_BPF_Array[for_rPD_BPFV + 1]);
			}
			rPD_BPFV_Avg = smusig->mean(rPD_BPFV_Array, 29);

			//rPD_BPFV_SD
			rPD_BPFV_SD = 0;
			rPD_BPFV_SD = smusig->standardDeviation(rPD_BPFV_Array, 29, 1);						//rPDV_SD

			///////////////////////////////////////////////////////////rPDV
			//rPDV_Avg
			rPDV_Avg = 0;
			rPDV_Avg = smusig->mean(rPDV_Array, 29);

			//rPDV_SD
			rPDV_SD = 0;
			rPDV_SD = smusig->standardDeviation(rPDV_Array, 29, 1);						//rPDV_SD

			//rPDV_BPF_SD 
			rPDV_BPF_SD = 0;
			smusig->bandpass(rPDV_Array, 1, 0.0833, 0.2, 2, rPDV_BPF_Array, 29);		///rPDV_BPF_Array
			rPDV_BPF_SD = smusig->standardDeviation(rPDV_BPF_Array, 29, 1);		//rPDV_BPF_SD

			//rPDV_BPF_Avg
			rPDV_BPF_Avg = 0;
			rPDV_BPF_Avg = smusig->mean(rPDV_BPF_Array, 29);

			//rPDV_BPFV_Avg
			rPDV_BPFV_Avg = 0;
			for (int for_rPDV_BPFV = 0; for_rPDV_BPFV < 28; for_rPDV_BPFV++) {
				rPDV_BPFV_Array[for_rPDV_BPFV] = abs(rPDV_BPF_Array[for_rPDV_BPFV] - rPDV_BPF_Array[for_rPDV_BPFV + 1]);
			}
			rPDV_BPFV_Avg = smusig->mean(rPDV_BPFV_Array, 28);

			//rPDV_BPFV_SD
			rPDV_BPFV_SD = 0;
			rPDV_BPFV_SD = smusig->standardDeviation(rPDV_BPFV_Array, 28, 1);		//rPDV_BPFV_SD

			////////////////////////////////////////////////////////////////////////////////////////////

			double HR_rPD = 0;
			HR_rPD_rule = 0;
			double Discriminant_Score = 0;

			HR_rPD = 1 / ((25 * rPD_BPFV_Avg) + 0.8);	//HR_rPD

			Discriminant_Score = (-0.036)*PD_Avg + (1.319)*PDV_BPF_SD + (-1.691)*PDV_BPFV_Avg + (1.639)* PDV_BPFV_SD + (8.534)*rPD_SD
				+ (59.686)*rPDV_Avg + (-23.987)*rPDV_SD + (-9.115)*rPDV_BPF_SD + (168.436)* rPDV_BPFV_Avg + (-337.121)*rPDV_BPFV_SD + 1.489;
			if (first_Dis == true)
			{
				first_Dis = false;
			}
			else
			{
				Discriminant_Score = (pre_Discriminant_Score + Discriminant_Score) / 2;
			}

			pre_Discriminant_Score = Discriminant_Score;
			if (Discriminant_Score <= -0.3) {
				if (0.035 <= rPD_BPFV_Avg) {
					HR_rPD_rule = HR_rPD + 0.3;
				}
				else if ((0.023 <= rPD_BPFV_Avg) && (rPD_BPFV_Avg < 0.035)) {
					HR_rPD_rule = HR_rPD + 0.15;
				}
				else if ((0.018 <= rPD_BPFV_Avg) && (rPD_BPFV_Avg < 0.023)) {
					HR_rPD_rule = HR_rPD + 0.06;
				}
				else if ((0.012 <= rPD_BPFV_Avg) && (rPD_BPFV_Avg < 0.018)) {
					HR_rPD_rule = HR_rPD;
				}
				else if (rPD_BPFV_Avg < 0.012) {
					HR_rPD_rule = HR_rPD - 0.012;
				}
			}
			else if ((-0.3 < Discriminant_Score) && (Discriminant_Score <= 0.24)) {
				if (0.035 <= rPD_BPFV_Avg) {
					HR_rPD_rule = HR_rPD + 0.2;
				}
				else if ((0.028 <= rPD_BPFV_Avg) && (rPD_BPFV_Avg < 0.035)) {
					HR_rPD_rule = HR_rPD + 0.11;
				}
				else if ((0.020 <= rPD_BPFV_Avg) && (rPD_BPFV_Avg < 0.028)) {
					HR_rPD_rule = HR_rPD + 0.04;
				}
				else if ((0.015 <= rPD_BPFV_Avg) && (rPD_BPFV_Avg < 0.020)) {
					HR_rPD_rule = HR_rPD;
				}
				else if (rPD_BPFV_Avg < 0.015) {
					HR_rPD_rule = HR_rPD - 0.15;
				}
			}
			else if (0.24 < Discriminant_Score) {
				if (0.0334 <= rPD_BPFV_Avg) {
					HR_rPD_rule = HR_rPD + 0.176;
				}
				else if ((0.026 <= rPD_BPFV_Avg) && (rPD_BPFV_Avg < 0.0334)) {
					HR_rPD_rule = HR_rPD + 0.083;
				}
				else if ((0.015 <= rPD_BPFV_Avg) && (rPD_BPFV_Avg < 0.026)) {
					HR_rPD_rule = HR_rPD + 0.01;
				}
				else if ((0.010 <= rPD_BPFV_Avg) && (rPD_BPFV_Avg < 0.015)) {
					HR_rPD_rule = HR_rPD - 0.1;
				}
				else if (rPD_BPFV_Avg < 0.010) {
					HR_rPD_rule = HR_rPD - 0.278;
				}
			}
			if (first_HR == true)
			{
				first_HR = false;
			}
			else
			{
				HR_rPD_rule = (pre_HR_rPD_rule + HR_rPD_rule) / 2;
			}

			pre_HR_rPD_rule = HR_rPD_rule;

			HR_rPD_Cnt++;

			if (_isnan(HR_rPD) || (!_finite(HR_rPD))){
				HR_rPD = 0;
			}

			if (_isnan(HR_rPD_rule) || (!_finite(HR_rPD_rule))){
				HR_rPD_rule = 0;
			}
			////////////////////////////////////////////////////////////////////////////////////////////
			for (int rawNum = 0; rawNum < (sampling_rate * 30) - 30; rawNum++) {
				rawdataPD[rawNum] = rawdataPD[rawNum + 30];
			}
			rawCount -= 30;
		}
		HR_frame_Cnt++;

		setrPD_HR(HR_rPD_rule);

		return HR_rPD_rule;
	}
	void SMUPupilDiameter_class::setrPD_SD(double rPD_SD){
		HR_rPD_SD = 0;
		HR_rPD_SD = rPD_SD;
		return;
	}
	double SMUPupilDiameter_class::getrPD_SD(){
		return HR_rPD_SD;
	}
	void SMUPupilDiameter_class::setrPD_HR(double rPD_HR){
		set_rPD_HR = rPD_HR;
		return;
	}
	double SMUPupilDiameter_class::getrPD_HR(){
		return set_rPD_HR;
	}

	//사회감성 개인내 - 경쟁협력 몰입도
	double SMUPupilDiameter_class::PupilDiameter_SE_w180(double radius) {
		double HR_w180_rPD_rule = 0;

		w180_rawdataPD[w180_rawCount] = (int)radius;
		//cout << "rawSECount: " << rawSECount << "\t" << rawdataw180_PD[rawSECount] << "\t" << radius << endl;
		w180_rawCount++;

		//30초부터 1초씩~

		if (w180_frame_Cnt % 30 == 0 && w180_frame_Cnt >= (30 * 180)) {
			//cout << "frameSE_Cnt : " << frameSE_Cnt << endl;
			int rawdataZeroCnt = 0;
			double rawdataSum = 0, rawdataAvg = 0, rawdataw180_PDSD = 0;

			//합계 => rawdataSum
			for (int rawNum = 0; rawNum < sampling_rate * 180; rawNum++) {
				rawdataSum += w180_rawdataPD[rawNum];
				if (w180_rawdataPD[rawNum] != 0) {
					w180_rawdataZeroPD[rawdataZeroCnt] = w180_rawdataPD[rawNum];
					rawdataZeroCnt++;
				}
			}

			//표준 편차
			rawdataAvg = (rawdataSum / rawdataZeroCnt);
			rawdataw180_PDSD = smusig->standardDeviation(w180_rawdataZeroPD, rawdataZeroCnt, 1);

			//2.5sigma - 이상치 결정
			memcpy(w180_copy_rawdataPD, w180_rawdataPD, sizeof(w180_rawdataPD));
			double rawdataw180_PDSDSum = 0, rawdataw180_PDSDCnt = 0, rawdataw180_PDSDAvg = 0;
			for (int rawNum = 0; rawNum < sampling_rate * 180; rawNum++) {
				if ((w180_copy_rawdataPD[rawNum] > (rawdataAvg + rawdataw180_PDSD * 2.5)) || (w180_copy_rawdataPD[rawNum] < (rawdataAvg - rawdataw180_PDSD * 2.5))) {
					w180_copy_rawdataPD[rawNum] = 0;
				}
			}

			//1Hz resampling && w180_PD_Avg
			w180_PD_Array_Sum = 0;
			for (int for_w180_PD_Avg = 0; for_w180_PD_Avg < 180; for_w180_PD_Avg++) {
				double Sum_1Hz = 0;
				int Cnt_1Hz = 0;

				for (int for_Raw_1Hz = for_w180_PD_Avg * 30; for_Raw_1Hz < (for_w180_PD_Avg + 1) * 30; for_Raw_1Hz++) {
					if (w180_copy_rawdataPD[for_Raw_1Hz] != 0) {
						Sum_1Hz += w180_copy_rawdataPD[for_Raw_1Hz];
						Cnt_1Hz++;
					}
				}
				if (Cnt_1Hz != 0) {
					w180_PD_Array[for_w180_PD_Avg] = Sum_1Hz / Cnt_1Hz;
				}
				else {
					w180_PD_Array[for_w180_PD_Avg] = 0;
				}
				w180_PD_Array_Sum += w180_PD_Array[for_w180_PD_Avg];

			}
			w180_PD_Avg = w180_PD_Array_Sum / 180;	//w180_PD_Avg

			//w180_PDV_Array
			for (int for_w180_PDV_Array = 0; for_w180_PDV_Array < 180 - 1; for_w180_PDV_Array++) {
				w180_PDV_Array[for_w180_PDV_Array] = abs(w180_PD_Array[for_w180_PDV_Array] - w180_PD_Array[for_w180_PDV_Array + 1]);
			}

			//w180_rPD_Array
			double w180_rPD_Array_MIN = 0;
			for (int for_w180_rPD_Array = 0; for_w180_rPD_Array < 180; for_w180_rPD_Array++) {
				w180_rPD_Array[for_w180_rPD_Array] = (w180_PD_Array[for_w180_rPD_Array] - w180_PD_Avg) / w180_PD_Avg;

				if (w180_rPD_Array_MIN > w180_rPD_Array[for_w180_rPD_Array]) {
					w180_rPD_Array_MIN = w180_rPD_Array[for_w180_rPD_Array];
				}
			}
			w180_rPD_Array_MIN = abs(w180_rPD_Array_MIN);
			//|w180_rPD|_Array
			for (int for_w180_rPD_Array = 0; for_w180_rPD_Array < 180; for_w180_rPD_Array++) {
				w180_rPD_Array[for_w180_rPD_Array] = w180_rPD_Array[for_w180_rPD_Array] + w180_rPD_Array_MIN;
			}
			//w180_rPDV_Array
			for (int for_w180_rPDV_Array = 0; for_w180_rPDV_Array < 180 - 1; for_w180_rPDV_Array++) {
				w180_rPDV_Array[for_w180_rPDV_Array] = abs(w180_rPD_Array[for_w180_rPDV_Array] - w180_rPD_Array[for_w180_rPDV_Array + 1]);
			}

			//w180_rPD_BPF_Avg
			w180_rPD_BPF_Avg = 0;
			smusig->bandpass(w180_rPD_Array, 1, 0.0833, 0.2, 2, w180_rPD_BPF_Array, 180);		///w180_rPDV_BPF_Array
			w180_rPD_BPF_Avg = smusig->mean(w180_rPD_BPF_Array, 180);

			//w180_rPD_BPF_SD
			w180_rPD_BPF_SD = 0;
			w180_rPD_BPF_SD = smusig->standardDeviation(w180_rPD_BPF_Array, 180, 1);						//w180_rPDV_SD


			//w180_rPD_BPFV_Avg
			w180_rPD_BPFV_Avg = 0;
			for (int for_w180_rPD_BPFV = 0; for_w180_rPD_BPFV < 180 - 1; for_w180_rPD_BPFV++) {
				w180_rPD_BPFV_Array[for_w180_rPD_BPFV] = abs(w180_rPD_BPF_Array[for_w180_rPD_BPFV] - w180_rPD_BPF_Array[for_w180_rPD_BPFV + 1]);
			}
			w180_rPD_BPFV_Avg = smusig->mean(w180_rPD_BPFV_Array, 180 - 1);
			////////////////////////////////////////////////////////////////////////////////////////////

			memcpy(NN_PSV_Array, w180_rPD_Array, sizeof(w180_rPD_Array));
			//NN=> PSV_Array => rPD_BPFV_Array

			double SDNN = 0;
			SDNN = smusig->standardDeviation(w180_rPD_BPFV_Array, 180 - 1, 1);

			double FFT_fs = (double)(180 * sampling_rate) / (double)180;
			double FFT_df = FFT_fs / (180 * sampling_rate);
			memset(NN_BPF, 0, 180);
			memset(NN_FFT, 0, 180 / 2);
			memset(VLF_NN_BPF, 0, 180);
			memset(VLF_NN_FFT, 0, 180 / 2);
			memset(HF_NN_BPF, 0, 180);
			memset(HF_NN_FFT, 0, 180 / 2);

			//NN_FFT
			smusig->bandpass(w180_rPD_Array, 1, 0.0033, 0.4, 2, NN_BPF, 180);
			memcpy(NN_FFT, smusig->fft(NN_BPF, 180, 180), 180 / 2);

			//VLF
			smusig->bandpass(w180_rPD_Array, 1, 0.0033, 0.04, 2, VLF_NN_BPF, 180);
			memcpy(VLF_NN_FFT, smusig->fft(VLF_NN_BPF, 180, 180), 180 / 2);
			double sum_VLF_NN_BPF = 0;
			for (int for_VLF_NN_BPF = 0; for_VLF_NN_BPF < (180 / 2); for_VLF_NN_BPF++) {
				sum_VLF_NN_BPF += VLF_NN_FFT[for_VLF_NN_BPF];
			}

			//HF
			double sum_HF_NN_BPF = 0;
			smusig->bandpass(w180_rPD_Array, 1, 0.15, 0.4, 2, HF_NN_BPF, 180);
			memcpy(HF_NN_FFT, smusig->fft(HF_NN_BPF, 180, 180), 180 / 2);
			for (int for_HF_NN_BPF = 0; for_HF_NN_BPF < (180 / 2); for_HF_NN_BPF++) {
				sum_HF_NN_BPF += HF_NN_FFT[for_HF_NN_BPF];
			}

			//rMSSD
			//squares_NN
			for (int for_squares_NN = 0; for_squares_NN < 180 - 1; for_squares_NN++) {
				squares_NN[for_squares_NN] = pow(w180_rPD_BPFV_Array[for_squares_NN], 2);
			}
			//평균한 다음
			double mean_squares_NN = 0;
			mean_squares_NN = smusig->mean(squares_NN, 180 - 1);
			//제곱근
			double sqrt_mean_squares_NN = 0;
			sqrt_mean_squares_NN = sqrt(mean_squares_NN);

			double rMSSD = 0;
			rMSSD = sqrt_mean_squares_NN;

			double intra_Competition = (log(SDNN * 1000) + log(sum_VLF_NN_BPF)) / log(sum_HF_NN_BPF);

			//NN=> PSV_Array => rPD_BPFV_Array, rPD_BPFV_Avg => RRI
			double intra_Focus = (w180_rPD_BPFV_Avg * 10) + log(rMSSD * 1000) + log(sum_HF_NN_BPF);

			////////////////////////////////////////////////////////////////////////////////////////////
			if (intra_Competition > intra_Competition_100){
				intra_Competition_100 = intra_Competition;
			}
			if (intra_Competition < intra_Competition_0){
				intra_Competition_0 = intra_Competition;
			}
			intra_Competition_P = abs(((intra_Competition - intra_Competition_0) / (intra_Competition_100 - intra_Competition_0)) * 100);

			if (intra_Competition_P > 100){
				intra_Competition_P = 100;
			}
			else if (intra_Competition_P < 0){
				intra_Competition_P = 0;
			}

			if (intra_Focus > intra_Focus_100){
				intra_Focus_100 = intra_Focus;
			}
			if (intra_Focus < intra_Focus_0){
				intra_Focus_0 = intra_Focus;
			}

			intra_Focus_P = abs(((intra_Focus - intra_Focus_0) / (intra_Focus_100 - intra_Focus_0)) * 100);

			if (intra_Focus_P > 100){
				intra_Focus_P = 100;
			}
			else if (intra_Focus_P < 0){
				intra_Focus_P = 0;
			}

			if (_isnan(intra_Competition) || (!_finite(intra_Competition))){
				intra_Competition = 0;
			}
			if (_isnan(intra_Competition_P) || (!_finite(intra_Competition_P))){
				intra_Competition_P = 0;
			}
			if (_isnan(intra_Focus) || (!_finite(intra_Focus))){
				intra_Focus = 0;
			}
			if (_isnan(intra_Focus_P) || (!_finite(intra_Focus_P))){
				intra_Focus_P = 0;
			}

			for (int rawNum = 0; rawNum < (sampling_rate * 180) - 30; rawNum++) {
				w180_rawdataPD[rawNum] = w180_rawdataPD[rawNum + 30];
			}
			w180_rawCount -= 30;
		}
		w180_frame_Cnt++;

		return ((int)intra_Competition_P * 1000 + (int)intra_Focus_P);
	}

	//사회감성 개인간 - 친밀도 공감도
	double SMUPupilDiameter_class::PupilDiameter_SE_w60(double radius) {
		//rPD_SD -> HR_PD_SD
		double rPD_SD = getrPD_SD();
		double intimacy_P = 0;
		double empathy_P = 0;
		double SE_w60_inter_correl_r = 0;

		//Social Emotion: 개인간(interindividual) - intimacy, empathy
		if ((SE_w60_frame_Cnt % 30 == 0) && (SE_w60_frame_Cnt < (30 * (30 + 30))) && SE_w60_frame_Cnt >= (30 * 30)) {   //30초부터 PD_SD(SDNN)값이 출력됨 ~ 59초까지 입력 받음
			SE_w60_SDNN_Array[SE_w60_SDNN_Array_Cnt] = rPD_SD;
			SE_w60_other_SDNN_Array[SE_w60_SDNN_Array_Cnt] = rPD_SD; //상대방의 SDNN 넣어야함, 임의로 나의 SDNN값 넣음
			SE_w60_SDNN_Array_Cnt++;
		}
		else if ((SE_w60_frame_Cnt % 30 == 0) && (SE_w60_frame_Cnt >= (30 * (30 + 30)))) {   //60초부터 1초마다 PD_SD(SDNN)을 받음
			for (int n = 0; n < 29; n++) {
				SE_w60_SDNN_Array[n] = SE_w60_SDNN_Array[n + 1];
				SE_w60_other_SDNN_Array[n] = SE_w60_other_SDNN_Array[n + 1];
			}
			SE_w60_SDNN_Array[29] = rPD_SD;
			SE_w60_other_SDNN_Array[29] = rPD_SD; //상대방의 SDNN 넣어야함, 임의로 나의 SDNN값 넣음

			SE_w60_inter_correl_r = smusig->corcoef(SE_w60_SDNN_Array, SE_w60_other_SDNN_Array, 30); // 상관계수 계산
			//cout << "rPD_SD: " <<rPD_SD <<", SE_w60_inter_correl_r: " << SE_w60_inter_correl_r <<  endl;

			//intimacy - intimacy_P
			intimacy_P = pow(SE_w60_inter_correl_r, 2) * 100;
			if (intimacy_P > 100) {
				intimacy_P = 100;
			}
			else if (intimacy_P < 0) {
				intimacy_P = 0;
			}
			//empathy - empathy_P
			empathy_P = pow(SE_w60_inter_correl_r, 2) * 100;
			if (empathy_P > 100) {
				empathy_P = 100;
			}
			else if (empathy_P < 0) {
				empathy_P = 0;
			}

		}
		///////////////////////////
		SE_w60_frame_Cnt++;

		return ((int)intimacy_P * 1000 + (int)empathy_P);
	}

	//사회감성 개인간 - 경쟁협력 몰입도
	double SMUPupilDiameter_class::PupilDiameter_SE_w120(double radius) {
		double inter_Competition_P = 0;
		double inter_High_Focus_P = 0;

		SE_w120_rawdataPD[SE_w120_rawCount] = (int)radius;
		SE_w120_rawCount++;

		//120초부터 1초씩~
		if (SE_w120_frame_Cnt % 120 == 0 && SE_w120_frame_Cnt >= (120 * 120)) {
			int rawdataZeroCnt = 0;
			double rawdataSum = 0, rawdataAvg = 0, rawdataPDSD = 0;

			//합계 => rawdataSum
			for (int rawNum = 0; rawNum < sampling_rate * 120; rawNum++) {
				rawdataSum += SE_w120_rawdataPD[rawNum];
				//cout << SE_w120_rawdataPD[rawNum] << endl;
				if (SE_w120_rawdataPD[rawNum] != 0) {
					SE_w120_rawdataZeroPD[rawdataZeroCnt] = SE_w120_rawdataPD[rawNum];
					//cout << SE_w120_rawdataZeroPD[rawdataZeroCnt] << endl;
					rawdataZeroCnt++;
				}
			}

			//표준 편차
			rawdataAvg = (rawdataSum / rawdataZeroCnt);
			rawdataPDSD = smusig->standardDeviation(SE_w120_rawdataZeroPD, rawdataZeroCnt, 1);
			//cout << rawdataAvg << "\t" << rawdataPDSD << endl;
			//cout << rawdataAvg << endl;

			//2.5sigma - 이상치 결정
			memcpy(SE_w120_copy_rawdataPD, SE_w120_rawdataPD, sizeof(SE_w120_rawdataPD));
			double rawdataPDSDSum = 0, rawdataPDSDCnt = 0, rawdataPDSDAvg = 0;
			for (int rawNum = 0; rawNum < sampling_rate * 120; rawNum++) {
				if ((SE_w120_copy_rawdataPD[rawNum] > (rawdataAvg + rawdataPDSD * 2.5)) || (SE_w120_copy_rawdataPD[rawNum] < (rawdataAvg - rawdataPDSD * 2.5))) {
					SE_w120_copy_rawdataPD[rawNum] = 0;
				}
			}

			//1Hz resampling && PD_Avg
			SE_w120_PD_Array_Sum = 0;
			for (int for_PD_Avg = 0; for_PD_Avg < 120; for_PD_Avg++) {
				double Sum_1Hz = 0;
				int Cnt_1Hz = 0;

				for (int for_Raw_1Hz = for_PD_Avg * 120; for_Raw_1Hz < (for_PD_Avg + 1) * 120; for_Raw_1Hz++) {
					if (SE_w120_copy_rawdataPD[for_Raw_1Hz] != 0) {
						Sum_1Hz += SE_w120_copy_rawdataPD[for_Raw_1Hz];
						Cnt_1Hz++;
					}
				}
				if (Cnt_1Hz != 0) {
					SE_w120_PD_Array[for_PD_Avg] = Sum_1Hz / Cnt_1Hz;
				}
				else {
					SE_w120_PD_Array[for_PD_Avg] = 0;
				}
				SE_w120_PD_Array_Sum += SE_w120_PD_Array[for_PD_Avg];

			}
			SE_w120_PD_Avg = SE_w120_PD_Array_Sum / 120;   //PD_Avg
			//cout << PD_Avg << endl;

			//PDV_Array
			for (int for_PDV_Array = 0; for_PDV_Array < 119; for_PDV_Array++) {
				SE_w120_PDV_Array[for_PDV_Array] = abs(SE_w120_PD_Array[for_PDV_Array] - SE_w120_PD_Array[for_PDV_Array + 1]);
			}

			//rPD_Array
			double rPD_Array_MIN = 0;
			for (int for_rPD_Array = 0; for_rPD_Array < 120; for_rPD_Array++) {
				SE_w120_rPD_Array[for_rPD_Array] = (SE_w120_PD_Array[for_rPD_Array] - SE_w120_PD_Avg) / SE_w120_PD_Avg;

				if (rPD_Array_MIN > SE_w120_rPD_Array[for_rPD_Array]) {
					rPD_Array_MIN = SE_w120_rPD_Array[for_rPD_Array];
				}
			}
			rPD_Array_MIN = abs(rPD_Array_MIN);
			//|rPD|_Array
			for (int for_rPD_Array = 0; for_rPD_Array < 120; for_rPD_Array++) {
				SE_w120_rPD_Array[for_rPD_Array] = SE_w120_rPD_Array[for_rPD_Array] + rPD_Array_MIN;
			}
			//rPDV_Array
			for (int for_rPDV_Array = 0; for_rPDV_Array < 119; for_rPDV_Array++) {
				SE_w120_rPDV_Array[for_rPDV_Array] = abs(SE_w120_rPD_Array[for_rPDV_Array] - SE_w120_rPD_Array[for_rPDV_Array + 1]);
			}
			//rPD_BPF_Avg
			SE_w120_rPD_BPF_Avg = 0;
			smusig->bandpass(SE_w120_rPD_Array, 1, 0.0833, 0.2, 2, SE_w120_rPD_BPF_Array, 120);      ///rPDV_BPF_Array
			SE_w120_rPD_BPF_Avg = smusig->mean(SE_w120_rPD_BPF_Array, 120);

			//rPD_BPF_SD
			SE_w120_rPD_BPF_SD = 0;
			SE_w120_rPD_BPF_SD = smusig->standardDeviation(SE_w120_rPD_BPF_Array, 120, 1);                  //rPDV_SD


			//rPD_BPFV_Avg
			SE_w120_rPD_BPFV_Avg = 0;
			for (int for_rPD_BPFV = 0; for_rPD_BPFV < 120 - 1; for_rPD_BPFV++) {
				SE_w120_rPD_BPFV_Array[for_rPD_BPFV] = abs(SE_w120_rPD_BPF_Array[for_rPD_BPFV] - SE_w120_rPD_BPF_Array[for_rPD_BPFV + 1]);
			}
			SE_w120_rPD_BPFV_Avg = smusig->mean(SE_w120_rPD_BPFV_Array, 120 - 1);
			///////////////////////////
			memcpy(SE_w120_NN_PSV_Array, SE_w120_rPD_Array, sizeof(SE_w120_rPD_Array));
			//NN=> PSV_Array => rPD_BPFV_Array

			double FFT_fs = (double)(120 * sampling_rate) / (double)120;
			double FFT_df = FFT_fs / (120 * sampling_rate);
			memset(SE_w120_VLF_NN_BPF, 0, 120);
			memset(SE_w120_VLF_NN_FFT, 0, 120 / 2);
			memset(SE_w120_LF_NN_BPF, 0, 120);
			memset(SE_w120_LF_NN_FFT, 0, 120 / 2);
			memset(SE_w120_HF_NN_BPF, 0, 120);
			memset(SE_w120_HF_NN_FFT, 0, 120 / 2);
			memset(SE_w120_TP_NN_BPF, 0, 120);
			memset(SE_w120_TP_NN_FFT, 0, 120 / 2);

			//VLF
			smusig->bandpass(SE_w120_rPD_Array, 1, 0.0033, 0.04, 2, SE_w120_VLF_NN_BPF, 120);
			memcpy(SE_w120_VLF_NN_FFT, smusig->fft(SE_w120_VLF_NN_BPF, 120, 120), 120 / 2);
			double sum_VLF_NN_BPF = 0;
			for (int for_VLF_NN_BPF = 0; for_VLF_NN_BPF < (120 / 2); for_VLF_NN_BPF++) {
				sum_VLF_NN_BPF += SE_w120_VLF_NN_FFT[for_VLF_NN_BPF];
			}

			//LF
			smusig->bandpass(SE_w120_rPD_Array, 1, 0.04, 0.15, 2, SE_w120_LF_NN_BPF, 120);
			memcpy(SE_w120_LF_NN_FFT, smusig->fft(SE_w120_LF_NN_BPF, 120, 120), 120 / 2);
			double sum_LF_NN_BPF = 0;
			for (int for_LF_NN_BPF = 0; for_LF_NN_BPF < (120 / 2); for_LF_NN_BPF++) {
				sum_LF_NN_BPF += SE_w120_LF_NN_FFT[for_LF_NN_BPF];
			}

			//HF
			double sum_HF_NN_BPF = 0;
			smusig->bandpass(SE_w120_rPD_Array, 1, 0.15, 0.4, 2, SE_w120_HF_NN_BPF, 120);
			memcpy(SE_w120_HF_NN_FFT, smusig->fft(SE_w120_HF_NN_BPF, 120, 120), 120 / 2);
			for (int for_HF_NN_BPF = 0; for_HF_NN_BPF < (120 / 2); for_HF_NN_BPF++) {
				sum_HF_NN_BPF += SE_w120_HF_NN_FFT[for_HF_NN_BPF];
			}

			//TP
			double sum_TP_NN_BPF = 0;
			double TP_DPF = -10;
			int TP_DPF_n = 0;
			smusig->bandpass(SE_w120_rPD_Array, 1, 0.0033, 0.4, 2, SE_w120_TP_NN_BPF, 120);
			memcpy(SE_w120_TP_NN_FFT, smusig->fft(SE_w120_TP_NN_BPF, 120, 120), 120 / 2);
			for (int for_TP_NN_BPF = 0; for_TP_NN_BPF < (120 / 2); for_TP_NN_BPF++) {
				sum_TP_NN_BPF += SE_w120_TP_NN_FFT[for_TP_NN_BPF];
				if (TP_DPF >= SE_w120_TP_NN_FFT[for_TP_NN_BPF]) {
					TP_DPF = SE_w120_TP_NN_FFT[for_TP_NN_BPF];
					TP_DPF_n = for_TP_NN_BPF;
				}
			}

			//SDNN
			double SDNN = 0;
			SDNN = smusig->standardDeviation(SE_w120_rPD_BPFV_Array, 120 - 1, 1);

			//rMSSD
			double rMSSD = 0;
			//squares_NN
			for (int for_squares_NN = 0; for_squares_NN < 120 - 1; for_squares_NN++) {
				SE_w120_squares_NN[for_squares_NN] = pow(SE_w120_rPD_BPFV_Array[for_squares_NN], 2);
			}
			//평균한 다음
			double mean_squares_NN = 0;
			mean_squares_NN = smusig->mean(SE_w120_squares_NN, 120 - 1);
			//제곱근
			double sqrt_mean_squares_NN = 0;
			sqrt_mean_squares_NN = sqrt(mean_squares_NN);
			rMSSD = sqrt_mean_squares_NN;

			//////////////////////////////////////////////////////
			//RRI, BPM, SDNN, rMSSD, LF, HF, LF(%), HF(%), LF/HF, log(LF), log(HF), log(LF)/log(HF), DPF, HRC(peakPower/totalPower-peakPower)
			int SE_w120_BPM = (int)(((60 / getrPD_HR()) > 0) ? (60 / getrPD_HR()) : 0);
			int SE_w120_RRI = 60 / SE_w120_BPM;
			//SDNN
			//rMSSD
			//LF -> sum_LF_NN_BPF
			//HF -> sum_HF_NN_BPF
			double LF_p = sum_LF_NN_BPF / sum_TP_NN_BPF;
			double HF_p = sum_HF_NN_BPF / sum_TP_NN_BPF;
			double LF_HF = sum_LF_NN_BPF / sum_HF_NN_BPF;
			double log_LF = log(sum_LF_NN_BPF);
			double log_HF = log(sum_HF_NN_BPF);
			double log_LF_log_HF = log_LF / log_HF;
			//DPF -> TP_DPF
			double HRC = SE_w120_TP_NN_FFT[TP_DPF_n] / (sum_TP_NN_BPF - SE_w120_TP_NN_FFT[TP_DPF_n]);
			//////////////////////////////////////////////////////
			//VLF, VLF(%), VLF/HF, DPF, HRC
			//VLF -> sum_VLF_NN_BPF
			double VLF_p = sum_VLF_NN_BPF / sum_TP_NN_BPF;
			double VLF_HF = sum_VLF_NN_BPF / sum_HF_NN_BPF;
			//DPF -> TP_DPF
			//HRC

			//Social Emotion: 개인간(interindividual) - 경쟁 협력(Competition), 몰입도(Focus)
			if ((HR_frame_Cnt % 30 == 0) && (HR_frame_Cnt < (30 * 150) && HR_frame_Cnt >= (30 * 120))) {   //120~150동안 저장
				SE_w120_BPM_Array[interindividua_Cnt] = SE_w120_BPM;
				SE_w120_RRI_Array[interindividua_Cnt] = SE_w120_RRI;
				SE_w120_SDNN_Array[interindividua_Cnt] = SDNN;
				SE_w120_rMSSD_Array[interindividua_Cnt] = rMSSD;
				SE_w120_sum_VLF_NN_BPF_Array[interindividua_Cnt] = sum_VLF_NN_BPF;
				SE_w120_sum_LF_NN_BPF_Array[interindividua_Cnt] = sum_LF_NN_BPF;
				SE_w120_sum_HF_NN_BPF_Array[interindividua_Cnt] = sum_HF_NN_BPF;
				SE_w120_VLF_p_Array[interindividua_Cnt] = VLF_p;
				SE_w120_LF_p_Array[interindividua_Cnt] = LF_p;
				SE_w120_HF_p_Array[interindividua_Cnt] = HF_p;
				SE_w120_LF_HF_Array[interindividua_Cnt] = LF_HF;
				SE_w120_log_LF_Array[interindividua_Cnt] = log_LF;
				SE_w120_log_HF_Array[interindividua_Cnt] = log_HF;
				SE_w120_log_LF_log_HF_Array[interindividua_Cnt] = log_LF_log_HF;
				SE_w120_TP_DPF_Array[interindividua_Cnt] = TP_DPF;
				SE_w120_HRC_Array[interindividua_Cnt] = HRC;

				other_SE_w120_BPM_Array[interindividua_Cnt] = SE_w120_BPM;
				other_SE_w120_RRI_Array[interindividua_Cnt] = SE_w120_RRI;
				other_SE_w120_SDNN_Array[interindividua_Cnt] = SDNN;
				other_SE_w120_rMSSD_Array[interindividua_Cnt] = rMSSD;
				other_SE_w120_sum_VLF_NN_BPF_Array[interindividua_Cnt] = sum_VLF_NN_BPF;
				other_SE_w120_sum_LF_NN_BPF_Array[interindividua_Cnt] = sum_LF_NN_BPF;
				other_SE_w120_sum_HF_NN_BPF_Array[interindividua_Cnt] = sum_HF_NN_BPF;
				other_SE_w120_VLF_p_Array[interindividua_Cnt] = VLF_p;
				other_SE_w120_LF_p_Array[interindividua_Cnt] = LF_p;
				other_SE_w120_HF_p_Array[interindividua_Cnt] = HF_p;
				other_SE_w120_LF_HF_Array[interindividua_Cnt] = LF_HF;
				other_SE_w120_log_LF_Array[interindividua_Cnt] = log_LF;
				other_SE_w120_log_HF_Array[interindividua_Cnt] = log_HF;
				other_SE_w120_log_LF_log_HF_Array[interindividua_Cnt] = log_LF_log_HF;
				other_SE_w120_TP_DPF_Array[interindividua_Cnt] = TP_DPF;
				other_SE_w120_HRC_Array[interindividua_Cnt] = HRC;

				interindividua_Cnt++;
			}
			else if ((HR_frame_Cnt % 30 == 0) && (HR_frame_Cnt >= (30 * (150)))) {   //60초부터 1초마다 PD_SD(SDNN)을 받음
				for (int n = 0; n < 29; n++) {
					SE_w120_BPM_Array[n] = SE_w120_BPM_Array[n + 1];
					SE_w120_RRI_Array[n] = SE_w120_RRI_Array[n + 1];
					SE_w120_SDNN_Array[n] = SE_w120_SDNN_Array[n + 1];
					SE_w120_rMSSD_Array[n] = SE_w120_rMSSD_Array[n + 1];
					SE_w120_sum_VLF_NN_BPF_Array[n] = SE_w120_sum_VLF_NN_BPF_Array[n + 1];
					SE_w120_sum_LF_NN_BPF_Array[n] = SE_w120_sum_LF_NN_BPF_Array[n + 1];
					SE_w120_sum_HF_NN_BPF_Array[n] = SE_w120_sum_HF_NN_BPF_Array[n + 1];
					SE_w120_VLF_p_Array[n] = SE_w120_VLF_p_Array[n + 1];
					SE_w120_LF_p_Array[n] = SE_w120_LF_p_Array[n + 1];
					SE_w120_HF_p_Array[n] = SE_w120_HF_p_Array[n + 1];
					SE_w120_LF_HF_Array[n] = SE_w120_LF_HF_Array[n + 1];
					SE_w120_log_LF_Array[n] = SE_w120_log_LF_Array[n + 1];
					SE_w120_log_HF_Array[n] = SE_w120_log_HF_Array[n + 1];
					SE_w120_log_LF_log_HF_Array[n] = SE_w120_log_LF_log_HF_Array[n + 1];
					SE_w120_TP_DPF_Array[n] = SE_w120_TP_DPF_Array[n + 1];
					SE_w120_HRC_Array[n] = SE_w120_HRC_Array[n + 1];

					other_SE_w120_BPM_Array[n] = other_SE_w120_BPM_Array[n + 1];
					other_SE_w120_RRI_Array[n] = other_SE_w120_RRI_Array[n + 1];
					other_SE_w120_SDNN_Array[n] = other_SE_w120_SDNN_Array[n + 1];
					other_SE_w120_rMSSD_Array[n] = other_SE_w120_rMSSD_Array[n + 1];
					other_SE_w120_sum_VLF_NN_BPF_Array[n] = other_SE_w120_sum_VLF_NN_BPF_Array[n + 1];
					other_SE_w120_sum_LF_NN_BPF_Array[n] = other_SE_w120_sum_LF_NN_BPF_Array[n + 1];
					other_SE_w120_sum_HF_NN_BPF_Array[n] = other_SE_w120_sum_HF_NN_BPF_Array[n + 1];
					other_SE_w120_VLF_p_Array[n] = other_SE_w120_VLF_p_Array[n + 1];
					other_SE_w120_LF_p_Array[n] = other_SE_w120_LF_p_Array[n + 1];
					other_SE_w120_HF_p_Array[n] = other_SE_w120_HF_p_Array[n + 1];
					other_SE_w120_LF_HF_Array[n] = other_SE_w120_LF_HF_Array[n + 1];
					other_SE_w120_log_LF_Array[n] = other_SE_w120_log_LF_Array[n + 1];
					other_SE_w120_log_HF_Array[n] = other_SE_w120_log_HF_Array[n + 1];
					other_SE_w120_log_LF_log_HF_Array[n] = other_SE_w120_log_LF_log_HF_Array[n + 1];
					other_SE_w120_TP_DPF_Array[n] = other_SE_w120_TP_DPF_Array[n + 1];
					other_SE_w120_HRC_Array[n] = other_SE_w120_HRC_Array[n + 1];
				}
				//사용자
				SE_w120_BPM_Array[29] = SE_w120_BPM;
				SE_w120_RRI_Array[29] = SE_w120_RRI;
				SE_w120_SDNN_Array[29] = SDNN;
				SE_w120_rMSSD_Array[29] = rMSSD;
				SE_w120_sum_VLF_NN_BPF_Array[29] = sum_VLF_NN_BPF;
				SE_w120_sum_LF_NN_BPF_Array[29] = sum_LF_NN_BPF;
				SE_w120_sum_HF_NN_BPF_Array[29] = sum_HF_NN_BPF;
				SE_w120_VLF_p_Array[29] = VLF_p;
				SE_w120_LF_p_Array[29] = LF_p;
				SE_w120_HF_p_Array[29] = HF_p;
				SE_w120_VLF_HF_Array[29] = VLF_HF;
				SE_w120_LF_HF_Array[29] = LF_HF;
				SE_w120_log_LF_Array[29] = log_LF;
				SE_w120_log_HF_Array[29] = log_HF;
				SE_w120_log_LF_log_HF_Array[29] = log_LF_log_HF;
				SE_w120_TP_DPF_Array[29] = TP_DPF;
				SE_w120_HRC_Array[29] = HRC;
				//상대방
				other_SE_w120_BPM_Array[29] = SE_w120_BPM;
				other_SE_w120_RRI_Array[29] = SE_w120_RRI;
				other_SE_w120_SDNN_Array[29] = SDNN;
				other_SE_w120_rMSSD_Array[29] = rMSSD;
				other_SE_w120_sum_VLF_NN_BPF_Array[29] = sum_VLF_NN_BPF;
				other_SE_w120_sum_LF_NN_BPF_Array[29] = sum_LF_NN_BPF;
				other_SE_w120_sum_HF_NN_BPF_Array[29] = sum_HF_NN_BPF;
				other_SE_w120_VLF_p_Array[29] = VLF_p;
				other_SE_w120_LF_p_Array[29] = LF_p;
				other_SE_w120_HF_p_Array[29] = HF_p;
				other_SE_w120_VLF_HF_Array[29] = VLF_HF;
				other_SE_w120_LF_HF_Array[29] = LF_HF;
				other_SE_w120_log_LF_Array[29] = log_LF;
				other_SE_w120_log_HF_Array[29] = log_HF;
				other_SE_w120_log_LF_log_HF_Array[29] = log_LF_log_HF;
				other_SE_w120_TP_DPF_Array[29] = TP_DPF;
				other_SE_w120_HRC_Array[29] = HRC;

				double SE_w120_BPM_correl_r = smusig->corcoef(SE_w120_BPM_Array, other_SE_w120_BPM_Array, 30);
				double SE_w120_RRI_correl_r = smusig->corcoef(SE_w120_RRI_Array, other_SE_w120_RRI_Array, 30);
				double SDNN_correl_r = smusig->corcoef(SE_w120_SDNN_Array, other_SE_w120_SDNN_Array, 30);
				double rMSSD_correl_r = smusig->corcoef(SE_w120_rMSSD_Array, other_SE_w120_rMSSD_Array, 30);
				double sum_VLF_NN_BPF_correl_r = smusig->corcoef(SE_w120_sum_VLF_NN_BPF_Array, other_SE_w120_sum_VLF_NN_BPF_Array, 30);
				double sum_LF_NN_BPF_correl_r = smusig->corcoef(SE_w120_sum_LF_NN_BPF_Array, other_SE_w120_sum_LF_NN_BPF_Array, 30);
				double sum_HF_NN_BPF_correl_r = smusig->corcoef(SE_w120_sum_HF_NN_BPF_Array, other_SE_w120_sum_HF_NN_BPF_Array, 30);
				double VLF_p_correl_r = smusig->corcoef(SE_w120_VLF_p_Array, other_SE_w120_VLF_p_Array, 30);
				double LF_p_correl_r = smusig->corcoef(SE_w120_LF_p_Array, other_SE_w120_LF_p_Array, 30);
				double HF_p_correl_r = smusig->corcoef(SE_w120_HF_p_Array, other_SE_w120_HF_p_Array, 30);
				double VLF_HF_correl_r = smusig->corcoef(SE_w120_VLF_HF_Array, other_SE_w120_VLF_HF_Array, 30);
				double LF_HF_correl_r = smusig->corcoef(SE_w120_LF_HF_Array, other_SE_w120_LF_HF_Array, 30);
				double log_LF_correl_r = smusig->corcoef(SE_w120_log_LF_Array, other_SE_w120_log_LF_Array, 30);
				double log_HF_correl_r = smusig->corcoef(SE_w120_log_HF_Array, other_SE_w120_log_HF_Array, 30);
				double log_LF_log_HF_correl_r = smusig->corcoef(SE_w120_log_LF_log_HF_Array, other_SE_w120_log_LF_log_HF_Array, 30);
				double TP_DPF_correl_r = smusig->corcoef(SE_w120_TP_DPF_Array, other_SE_w120_TP_DPF_Array, 30);
				double HRC_correl_r = smusig->corcoef(SE_w120_HRC_Array, other_SE_w120_HRC_Array, 30);

				//inter_Competition_P
				inter_Competition_P = ((SE_w120_BPM_correl_r + SE_w120_RRI_correl_r + SDNN_correl_r + rMSSD_correl_r + sum_LF_NN_BPF_correl_r + sum_HF_NN_BPF_correl_r
					+ LF_p_correl_r + HF_p_correl_r + LF_HF_correl_r + log_LF_correl_r + log_HF_correl_r + log_LF_log_HF_correl_r + TP_DPF_correl_r + HRC_correl_r) / 14) * 100;
				if (inter_Competition_P > 100) {
					inter_Competition_P = 100;
				}
				else if (inter_Competition_P < 0) {
					inter_Competition_P = 0;
				}
				//inter_High_Focus_P
				inter_High_Focus_P = ((sum_VLF_NN_BPF_correl_r + VLF_p_correl_r + VLF_HF_correl_r) / 3) / ((TP_DPF_correl_r + HRC_correl_r) / 2);
				if (inter_High_Focus_P > 100) {
					inter_High_Focus_P = 100;
				}
				else if (inter_High_Focus_P < 0) {
					inter_High_Focus_P = 0;
				}
			}
			///////////////////////////
			for (int rawNum = 0; rawNum < (sampling_rate * 120) - 120; rawNum++) {
				SE_w120_rawdataPD[rawNum] = SE_w120_rawdataPD[rawNum + 120];
			}
			SE_w120_rawCount -= 120;
		}

		SE_w120_frame_Cnt++;


		return ((int)inter_Competition_P * 1000 + (int)inter_High_Focus_P);;
	}

}
