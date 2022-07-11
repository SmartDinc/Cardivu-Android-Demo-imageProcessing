#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/log.h>
#include <math.h>
#include <cstring>
#include <string.h>
#include<fstream>
#include <iosfwd>

using namespace cv;

double
drawOptFlowMap(const Mat &flow, Mat &cflowmap, int step, double, const Scalar &color, int labelNum);

double optical_Flow(Mat &frame);

Mat getHistogram(const Mat &src, double *minVal, double *maxVal);

void thresVertical(Mat src, Mat &dst, int thres);

float translateN(float n, float src_len, float dst_len);

bool drawCirlceWith3P(Mat dst, Point p1, Point p2, Point p3, Point2d &center, double &radius);

void cannyVertical(Mat src, Mat &dst, int threshold1, int threshold2);

bool inCircle(Vec3d circle1, Vec3d circle2);

void detectIris(Mat gray, Mat &dst, int mode);

void cvtContrast(Mat src, Mat &dst, int brightness, int contrast);

std::vector<cv::Point2f> getPointPositions(cv::Mat binaryImage);

void sharpImage(Mat src, Mat &dst, int size);

bool drawCircleWith3P(int width, int height, Point2i p1, Point2i p2, Point2i p3, Point2d &center,
                      double &radius);

Mat getPointPositions2(Mat binaryImage);

const int RE_SIZE = 150;

bool findp = false;
bool drawp = false;
float rr = 0;
bool firstIRIS = true;
bool before_op = true;
int iris_width = 0;
double OF_GAvg = 0;
double illAvg = 0;
float sersor_Lux = 0;

Point2f cc(0, 0);
Mat origMat;    //Mat : 디지털화된 이미지를 처리할 수 있는 클래스
Mat img_thres;
Rect2f faceRect;
Rect2f lEyeRect;
Rect2f rEyeRect;
Point2f irisC;
Point rEyeCenter;
Rect illuRect;

UMat gray_1Q, gray_2Q, gray_3Q, gray_4Q, gray_Avg, prevgray_1Q, prevgray_2Q, prevgray_3Q, prevgray_4Q, prevgray_Avg;        // LDW 191106
Mat flow_1Q, flow_2Q, flow_3Q, flow_4Q, flow_Avg, cflow_1Q, cflow_2Q, cflow_3Q, cflow_4Q, cflow_Avg;        // LDW 191106
UMat uflow_1Q, uflow_2Q, uflow_3Q, uflow_4Q, uflow_Avg;     // LDW 191106
UMat grayF, prevgrayF, gray_34Avg, prevgray_34Avg;      // LDW 191106
Mat flowF, cflowF, flow_34Avg, cflow_34Avg;      // LDW 191106
UMat uflowF, uflow_34Avg;      // LDW 191106

float OFVars[244];
int frameCnt_returnPupilSize = 0;

VideoCapture cap1;
bool fristStart = true;
int videoFileProcessing_frameCnt = 0;

extern "C"
JNIEXPORT void JNICALL
Java_com_sd_cardivu_1dev_1beta_MeasureActivity_videoFileProcessing(JNIEnv *env, jobject thiz,
                                                                   jlong matAddrFile,
                                                                   jstring filepath, jint h, jint w,
                                                                   jint flip_n) {

    Mat &matFile = *(Mat *) matAddrFile;

    if (fristStart) {
//        cap1.open("/storage/emulated/0/Download/SDData/android.mp4");
        const char *nativeString = env->GetStringUTFChars(filepath, 0);
        cap1.open(nativeString);

        if (!cap1.isOpened()) {
            __android_log_print(ANDROID_LOG_ERROR, "동영상 파일을 열수 없습니다.", "%d, %d", 0, 0);
        }
        fristStart = false;
    }

    Mat frame1;
    cap1 >> frame1;
    resize(frame1.clone(), matFile, Size(h, w));

    if (flip_n != 1204) {
        flip(frame1.clone(), matFile, flip_n);
    }


/*
Mat buffFrame1;
cvtColor(frame1, buffFrame1, COLOR_RGB2BGR);
char sstring[] = "";
sprintf(sstring,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/frame", videoFileProcessing_frameCnt,".jpg");
videoFileProcessing_frameCnt = videoFileProcessing_frameCnt + 1;
cv::imwrite(sstring, buffFrame1);
 */
//    [출처] C++ OpenCV imwrite 함수 ( 이미지 저장 )|작성자 영산홍
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_net_sdcor_sdapi_imageprocmodule_cppNetiveImageProcessing_returnPupilSize(JNIEnv *env,
                                                                              jobject thiz,
                                                                              jlong mat_addr_input,
                                                                              jintArray illu,
                                                                              jfloatArray face,
                                                                              jfloatArray left_eye,
                                                                              jfloatArray rightEye,
                                                                              jfloatArray irisCenter,
                                                                              jfloatArray OF_arry,
                                                                              jfloat ref_radius,
                                                                              jintArray RGB_val) {
    try {
//    __android_log_print(ANDROID_LOG_ERROR, "returnPupilSize ", "%d, %d", 0, 0);
// TODO: implement returnPupilSize()


        Mat &matInput = *(Mat *) mat_addr_input;

        jfloat *ptrREyeArray = env->GetFloatArrayElements(rightEye, 0);
//0~3:rect(p1.x,p1.y,p2.x,p2.y)  4~5:right eye's landmark point
        jfloat *ptrIrisCenter = env->GetFloatArrayElements(irisCenter, 0);
//iris center point and radius
        jfloat *ptrOFArry = env->GetFloatArrayElements(OF_arry, 0);//244개 변수      

        jint *rgb_valArry = env->GetIntArrayElements(RGB_val, 0);

        rEyeRect = Rect2f(Point2f(ptrREyeArray[0], ptrREyeArray[1]),
                          Point2f(ptrREyeArray[2], ptrREyeArray[3]));
        rEyeCenter = Point2f(ptrREyeArray[4], ptrREyeArray[5]);

//    rEyeRect = Rect2f(707 - 90, 398 - 3, 90, 90);
//    rEyeCenter = Point2f(707 - 45, 443 - 3);

        if (matInput.rows > 0 &&
            (rEyeRect.x >= 0 && rEyeRect.x + rEyeRect.width < matInput.cols && rEyeRect.y >= 0 &&
             rEyeRect.y + rEyeRect.height < matInput.rows)) {

            int rows = matInput.rows;
            int cols = matInput.cols;
//        __android_log_print(ANDROID_LOG_ERROR, "matInput Size", "%d, %d", rows, cols);


            frameCnt_returnPupilSize = frameCnt_returnPupilSize + 1;

//RY - Android 영상처리 적용 2020.06.03
            Mat eyeM = matInput(rEyeRect).clone(); //오른쪽 눈 영역으로 자른 이미지

//        Mat image;
//        image = imread("/storage/emulated/0/Download/SDData/testimg.jpg");
//        if (image.empty())
//            return;
//        eyeM = image.clone();

//오른쪽 눈 영역 리사이즈 200x200
            Mat rszEyeM;
            resize(eyeM, rszEyeM, Size(200, 200));

//-- 1. resized eye roi
////        resultMat = rszEyeM.clone();
//        try {
////            __android_log_print(ANDROID_LOG_ERROR, "buff_resized :0_", "%d, %d", 0, 0);
//            Mat buff_resized;
////            __android_log_print(ANDROID_LOG_ERROR, "buff_resized :1_", "%d, %d", 0, 0);
////            cvtColor(rszEyeM.clone(), buff_resized, COLOR_RGB2BGR);
//            buff_resized = rszEyeM.clone();
//            cvtColor(rszEyeM.clone(), buff_resized, COLOR_RGB2BGR);
//            flip(buff_resized.clone(), buff_resized, 1);
////            __android_log_print(ANDROID_LOG_ERROR, "buff_resized :2_", "%d, %d", 0, 0);
//            char str_buff_resized[] = "";
////            __android_log_print(ANDROID_LOG_ERROR, "buff_resized :3_", "%d, %d", 0, 0);
//            sprintf(str_buff_resized, "%s_%d_%s", "/storage/emulated/0/Download/SDData/test/_1_",
//                    frameCnt_returnPupilSize, "_resized_.jpg");
////            __android_log_print(ANDROID_LOG_ERROR, "buff_resized :4_", "%d, %d", 0, 0);
//            cv::imwrite(str_buff_resized, buff_resized);
////            __android_log_print(ANDROID_LOG_ERROR, "buff_resized :5_", "%d, %d", 0, 0);
//        } catch (const std::exception &e) {
////            __android_log_print(ANDROID_LOG_ERROR, "buff_resized :6_", "%d, %d", 0, 0);
////            __android_log_print(ANDROID_LOG_ERROR, "exception-buff_resized :", "%c, %d", e.what(),
////                                0);
//            std::cerr << "exception: " << e.what() << std::endl;
//        }


            Mat rgb[3], rM; // R영역 이미지
            std::vector<Mat> vec_rgb;
            split(rszEyeM, vec_rgb);   //채널 분리

//-- 2. split red channel
//        resultMat = vec_rgb[0].clone();
//        Mat buff_split_red;
//        buff_split_red = vec_rgb[2].clone();
//        char str_split_red[] = "";
//        sprintf(str_split_red,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_2_", frameCnt_returnPupilSize,"_split_red2_.jpg");
//        cv::imwrite(str_split_red, buff_split_red);

//        normalize(vec_rgb[0], rM, 0, 255, NORM_MINMAX);
            normalize(vec_rgb[2], rM, 0, 255, NORM_MINMAX);

//-- 3. normalize red
//        resultMat = rM.clone();
//        Mat buff_normalize_red;
//        buff_normalize_red = rM.clone();
//        char str_normalize_red[] = "";
//        sprintf(str_normalize_red,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_3_", frameCnt_returnPupilSize,"_normalize_red_.jpg");
//        cv::imwrite(str_normalize_red, buff_normalize_red);

// 홍채 주변을 밝게해서 홍채가 가장 어두워보이게 하기위함, 특히 눈 앞머리를 밝게 하기위함
            Mat gradationBlur;
            Mat afterR = rM.clone();
            blur(afterR, gradationBlur, Size(afterR.rows / 4, afterR.cols / 4));

//-- 4. blur red
//        resultMat = gradationBlur.clone();
//        Mat buff_blur_red;
//        buff_blur_red = gradationBlur.clone();
//        char str_blur_red[] = "";
//        sprintf(str_blur_red,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_4_", frameCnt_returnPupilSize,"_blur_red_.jpg");
//        cv::imwrite(str_blur_red, buff_blur_red);

            morphologyEx(gradationBlur, gradationBlur, MORPH_ERODE,
                         getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));

//-- 5. MORPH_ERODE blur
//        resultMat = gradationBlur.clone();
//        Mat buff_MORPH_ERODE;
//        buff_MORPH_ERODE = gradationBlur.clone();
//        char str_MORPH_ERODE[] = "";
//        sprintf(str_MORPH_ERODE,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_5_", frameCnt_returnPupilSize,"_MORPH_ERODE_.jpg");
//        cv::imwrite(str_MORPH_ERODE, buff_MORPH_ERODE);

            normalize(gradationBlur, gradationBlur, 0, 255, NORM_MINMAX);

//-- 6. normalize blur
//        resultMat = gradationBlur.clone();
//        Mat buff_normalize_blur;
//        buff_normalize_blur = gradationBlur.clone();
//        char str_normalize_blur[] = "";
//        sprintf(str_normalize_blur,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_6_", frameCnt_returnPupilSize,"_normalize_blur_.jpg");
//        cv::imwrite(str_normalize_blur, buff_normalize_blur);

            addWeighted(gradationBlur, 0.6, afterR, 1.0, 0, afterR);    //눈 주위 영역 밝힘

//-- 7. addWeighted blur and red
//        resultMat = afterR.clone();
//        Mat buff_addWeighted_blur_and_red;
//        buff_addWeighted_blur_and_red = afterR.clone();
//        char str_addWeighted_blur_and_red[] = "";
//        sprintf(str_addWeighted_blur_and_red,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_7_", frameCnt_returnPupilSize,"_addWeighted_blur_and_red_.jpg");
//        cv::imwrite(str_addWeighted_blur_and_red, buff_addWeighted_blur_and_red);

// 이미지 강화(선명)
            Mat sharpM;
            sharpImage(afterR, sharpM, 3);

//-- 8. sharp red1
//        resultMat = sharpM.clone();
//        Mat buff_sharp_red1;
//        buff_sharp_red1 = sharpM.clone();
//        char str_sharp_red1[] = "";
//        sprintf(str_sharp_red1,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_8_", frameCnt_returnPupilSize,"_sharp_red1_.jpg");
//        cv::imwrite(str_sharp_red1, buff_sharp_red1);

//        Mat sharpM2 = sharpM - (~sharpM) * 0.5;
            Mat sharpM2;// = sharpM - (~sharpM) * 0.5;
            addWeighted(sharpM, 1, ~sharpM, -0.5, 0, sharpM2); //RY 200703

//-- 9. sharp2 red
//resultMat = sharpM2.clone();
//        Mat buff_sharp2_red;
//        buff_sharp2_red = sharpM2.clone();
//        char str_sharp2_red[] = "";
//        sprintf(str_sharp2_red,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_9_", frameCnt_returnPupilSize,"_sharp2_red_.jpg");
//        cv::imwrite(str_sharp2_red, buff_sharp2_red);

// 홍채찾기 알고리즘을 적용할 영상
            Mat rstM;
//        rstM = (rM * 0.5) + sharpM2;    //강화한 이미지 + 원본 이미지 = 홍채 영역이 좀 더 강화된 이미지
            addWeighted(rM, 0.5, sharpM2, 1, 0,
                        rstM); //강화한 이미지 + 원본 이미지 = 홍채 영역이 좀 더 강화된 이미지. RY 200703

//-- 10. sharp3 red
//        resultMat = rstM.clone();
//        Mat buff_sharp3_red;
//        buff_sharp3_red = rstM.clone();
//        char str_sharp3_red[] = "";
//        sprintf(str_sharp3_red,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_10_", frameCnt_returnPupilSize,"_sharp3_red_.jpg");
//        cv::imwrite(str_sharp3_red, buff_sharp3_red);

            normalize(rstM, rstM, 0, 255, NORM_MINMAX);

//-- 11. normalize
//        resultMat = rstM.clone();
//        Mat buff_11_normalize;
//        buff_11_normalize = rstM.clone();
//        char str_11_normalize[] = "";
//        sprintf(str_11_normalize,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_11_", frameCnt_returnPupilSize,"_normalize_.jpg");
//        cv::imwrite(str_11_normalize, buff_11_normalize);

// canny Map - 전체적인 윤곽선
            Mat cannyM;
            morphologyEx(rstM, rstM, MORPH_OPEN, getStructuringElement(MORPH_ELLIPSE, Size(9, 9)));

//-- 12. MORPH_OPEN
//        resultMat = rstM.clone();
//        Mat buff_MORPH_OPEN;
//        buff_MORPH_OPEN = rstM.clone();
//        char str_MORPH_OPEN[] = "";
//        sprintf(str_MORPH_OPEN,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_12_", frameCnt_returnPupilSize,"_MORPH_OPEN_.jpg");
//        cv::imwrite(str_MORPH_OPEN, buff_MORPH_OPEN);

            GaussianBlur(rstM, cannyM, Size(3, 3), 0);

//-- 13. GaussianBlur
//        resultMat = cannyM.clone();
//        Mat buff_13_GaussianBlur;
//        buff_13_GaussianBlur = cannyM.clone();
//        char str_13_GaussianBlur[] = "";
//        sprintf(str_13_GaussianBlur,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_13_", frameCnt_returnPupilSize,"_GaussianBlur_.jpg");
//        cv::imwrite(str_13_GaussianBlur, buff_13_GaussianBlur);

            Canny(cannyM, cannyM, 10, 200);

//-- 14. Canny
//        resultMat = cannyM.clone();
//        Mat buff_Canny;
//        buff_Canny = cannyM.clone();
//        char str_Canny[] = "";
//        sprintf(str_Canny,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_14_", frameCnt_returnPupilSize,"_Canny_.jpg");
//        cv::imwrite(str_Canny, buff_Canny);

// gradient Map - 전체적인 윤곽선 2
            Mat rgradient;
            morphologyEx(rstM, rgradient, MORPH_GRADIENT,
                         getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));

//-- 15. MORPH_GRADIENT
//        resultMat = rgradient.clone();
//        Mat buff_MORPH_GRADIENT;
//        buff_MORPH_GRADIENT = rgradient.clone();
//        char str_MORPH_GRADIENT[] = "";
//        sprintf(str_MORPH_GRADIENT,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_15_", frameCnt_returnPupilSize,"_MORPH_GRADIENT_.jpg");
//        cv::imwrite(str_MORPH_GRADIENT, buff_MORPH_GRADIENT);

            normalize(rgradient, rgradient, 0, 255, NORM_MINMAX);

//-- 16. normalize
//        resultMat = rgradient.clone();
//        Mat buff_16_normalize;
//        buff_16_normalize = rgradient.clone();
//        char str_16_normalize[] = "";
//        sprintf(str_16_normalize,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_16_", frameCnt_returnPupilSize,"_normalize_.jpg");
//        cv::imwrite(str_16_normalize, buff_16_normalize);

            threshold(rgradient, rgradient, 50, 255, THRESH_BINARY);

//-- 17. threshold
//        resultMat = rgradient.clone();
//        Mat buff_17_threshold;
//        buff_17_threshold = rgradient.clone();
//        char str_17_threshold[] = "";
//        sprintf(str_17_threshold,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_17_", frameCnt_returnPupilSize,"_threshold_.jpg");
//        cv::imwrite(str_17_threshold, buff_17_threshold);


// 윤곽선 1과 2가 공통적으로 가지고 있는 윤곽선
            Mat contourM;
            bitwise_and(cannyM, rgradient, contourM);

//-- 18. bitwise_and
//        resultMat = contourM.clone();
//        Mat buff_bitwise_and;
//        buff_bitwise_and = contourM.clone();
//        char str_bitwise_and[] = "";
//        sprintf(str_bitwise_and,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_18_", frameCnt_returnPupilSize,"_bitwise_and_.jpg");
//        cv::imwrite(str_bitwise_and, buff_bitwise_and);


//iris radius size
            int MAX_R = 40;
            int MIN_R = 25;

//canny map 노이즈 지우기
            std::vector<std::vector<Point>> rcontours;
            findContours(contourM, rcontours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
            for (int i = 0; i < rcontours.size(); i++) {
                if (rcontours[i].size() > 10) {
//RotatedRect el = fitEllipse(rcontours[i]);
                    Point2f center;
                    float radius;
                    minEnclosingCircle(rcontours[i], center, radius);
                    if (radius > MIN_R && radius < MAX_R) {
//circle(color1, center, radius, Scalar(0, 0, 255));
                    }
                } else {
                    drawContours(contourM, rcontours, i, Scalar(0, 0, 0), -1);
                }
            }

//-- 19. drawContours
//        resultMat = contourM.clone();
//        Mat buff_drawContours;
//        buff_drawContours = contourM.clone();
//        char str_drawContours[] = "";
//        sprintf(str_drawContours,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_19_", frameCnt_returnPupilSize,"_drawContours_.jpg");
//        cv::imwrite(str_drawContours, buff_drawContours);


            Point2f irisC_(translateN(rEyeCenter.x - rEyeRect.x, rEyeRect.width, 200.f),
                           translateN(rEyeCenter.y - rEyeRect.y, rEyeRect.width, 200.f));

//        Point2f irisC_(100, 100);

//홍채 점 후보 구하기
            std::vector<Vec3f> candi_points;

            Mat pointsM = contourM.clone();
            pointsM = Scalar::all(0);

            for (int p = -1 * MAX_R; p < MAX_R * 4; p += 1) {
//왼쪽
                LineIterator it(contourM, irisC_,
                                Point2d(irisC_.x - MAX_R * 1.5 > 0 ? irisC_.x - MAX_R * 1.5 : 15,
                                        irisC_.y + p));
                for (int r = 15; r < it.count; r++, ++it) {
                    if (**it > 0) {
//candi_points.push_back(Vec3f(it.pos().x, it.pos().y, r));
                        circle(pointsM, Point(it.pos().x, it.pos().y), 1, Scalar(255, 255, 255),
                               -1);
                        break;
                    }
                }
//오른쪽
                LineIterator irt(contourM, irisC_,
                                 Point2d(irisC_.x + MAX_R * 1.5 < rszEyeM.rows ? irisC_.x +
                                                                                 MAX_R * 1.5
                                                                               : rszEyeM.rows - 15,
                                         irisC_.y + p));
                for (int r = 0; r < irt.count; r++, ++irt) {
                    if (**irt > 0) {
//candi_points.push_back(Vec3f(irt.pos().x, irt.pos().y, r));
                        circle(pointsM, Point(irt.pos().x, irt.pos().y), 1, Scalar(255, 255, 255),
                               -1);
                        break;
                    }
                }
            }

//-- 20. pointsM
//        resultMat = pointsM.clone();
//        Mat buff_pointsM;
//        buff_pointsM = pointsM.clone();
//        char str_pointsM[] = "";
//        sprintf(str_pointsM,"%s_%d_%s","/storage/emulated/0/Download/SDData/test/_20_", frameCnt_returnPupilSize,"_pointsM_.jpg");
//        cv::imwrite(str_pointsM, buff_pointsM);


// 홍채 원 후보 구하기
            std::vector<Vec3f> candi_circles;//홍채원 후보 벡터

//Mat colorM;
//cvtColor(contourM, colorM, COLOR_GRAY2RGB);

//vector<Point2f> points = getPointPositions(pointsM);
//        for (int j = 0; j < points.size(); j++) {
//            Point2d ce;
//            double r;
//            int a, b, c, d;
//            a = rand() % points.size();
//            b = rand() % points.size();
//            c = rand() % points.size();
//            d = rand() % points.size();
//
//            if (a == b && b == c && c == d) continue;
//
//            if (drawCirlceWith3P(colorM, points[a], points[b], points[c], ce, r)) {
//                if (r > MIN_R && r < MAX_R && (pow(points[d].x - ce.x, 2) + pow(points[d].y - ce.y, 2) < r * r)) {
//                    //circle(color3, ce, r, Scalar(255, 255, 0), 1);
//                    candi_circles.push_back(Vec3f(ce.x, ce.y, r));
//                }
//            }
//        }

//        cout<<"pointsM"<<endl;
//        for (int y = 0; y < pointsM.rows; y++) {
//            unsigned char *ptr = pointsM.ptr<uchar>(y);
//            for (int x = 0; x < pointsM.cols; x++) {
//                if(ptr[x]>0){
//                    cout<<1<<" ";
//                }else{
//                    cout<<0<<" ";
//                }
//
//            }
//            cout<<"!";
//        }
//        cout<<endl;

//        Text 저장
//        char str_buff__pointsM[] = "";
//        sprintf(str_buff__pointsM, "%s", "/storage/emulated/0/Download/SDData/test/_Total_pointsM_.txt");
//
////        std::ofstream writeFile;
////        writeFile.open(str_buff__pointsM);
//        std::ofstream writeFile(str_buff__pointsM, std::ios_base::out | std::ios_base::app);
//
//        writeFile << "pointsM: \n";
//        for (int y = 0; y < pointsM.rows; y++) {
//            unsigned char *ptr = pointsM.ptr<uchar>(y);
//            for (int x = 0; x < pointsM.cols; x++) {
//                if (ptr[x] > 0) {
//                    writeFile << 1 << " ";
//                } else {
//                    writeFile << 0 << " ";
//                }
//
//            }
//            writeFile << "\t@\n";
//        }
//        writeFile << "\n" << frameCnt_returnPupilSize << "\n\n";


//        출처: https://blockdmask.tistory.com/322 [개발자 지망생]

//-- 21-5. points2
//            resultMat = rIrisRectMat.clone();

//        try {
//            Mat buff_points2;
//            __android_log_print(ANDROID_LOG_ERROR, "buff_points2 :1_", "%d, %d", 0, 0);
//            buff_points2 = points2.clone();
//            __android_log_print(ANDROID_LOG_ERROR, "buff_points2 :2_", "%d, %d", 0, 0);
//            char str_points2[] = "";
//            __android_log_print(ANDROID_LOG_ERROR, "buff_points2 :3_", "%d, %d", 0, 0);
//            sprintf(str_points2, "%s_%d_%s", "/storage/emulated/0/Download/SDData/test/_21_5_",
//                    frameCnt_returnPupilSize, "_points2_.jpg");
//            +__android_log_print(ANDROID_LOG_ERROR, "buff_points2 :4_", "%d, %d", 0, 0);
//            cv::imwrite(str_points2, buff_points2);
//            __android_log_print(ANDROID_LOG_ERROR, "buff_points2 :5_", "%d, %d", 0, 0);
//        } catch (const std::exception &e) {
//            __android_log_print(ANDROID_LOG_ERROR, "buff_points2 :6_", "%d, %d", 0, 0);
//            __android_log_print(ANDROID_LOG_ERROR, "exception-buff_points2 :", "%c, %d", e.what(),
//                                0);
//            std::cerr << "exception: " << e.what() << std::endl;
//        }

// 라영누나 버전
            Mat points2 = getPointPositions2(pointsM);

            int pointsSize = (int) points2.total();
            for (int j = 0; j < pointsSize; j++) {
                Point2d ce;
                double r;
                int a, b, c, d;
                a = rand() % pointsSize;
                b = rand() % pointsSize;
                c = rand() % pointsSize;
                d = rand() % pointsSize;
                if (a == b && b == c && c == d) continue;

                if (drawCircleWith3P(pointsM.cols, pointsM.rows, points2.at<Point>(a),
                                     points2.at<Point>(b), points2.at<Point>(c), ce, r)) {
                    if (r > MIN_R && r < MAX_R &&
                        (pow(points2.at<Point>(d).x - ce.x, 2) +
                         pow(points2.at<Point>(d).y - ce.y, 2) <
                         r * r)) {
//circle(color3, ce, r, Scalar(255, 255, 0), 1);
                        candi_circles.push_back(Vec3f(ce.x, ce.y, r));
                    }
                }
            }

//        __android_log_print(ANDROID_LOG_ERROR, "getPointPositions ", "%d, %d", 0, 0);

/*
// 기존 버전
std::vector<Point2f> points = getPointPositions(pointsM);
for (int j = 0; j < points.size(); j++) {
    Point2d ce;
    double r;
    int a, b, c, d;

    a = rand() % points.size();
    b = rand() % points.size();
    c = rand() % points.size();
    d = rand() % points.size();
    if (a == b && b == c && c == d) continue;
    if (drawCirlceWith3P(pointsM, points[a], points[b], points[c], ce, r)) {
        if (r > MIN_R && r < MAX_R  && (pow(points[d].x - ce.x, 2) + pow(points[d].y - ce.y, 2) < r * r)) {
            candi_circles.push_back(Vec3f(ce.x, ce.y, r));
        }
    }
}
// 기존 버전
 */

//        __android_log_print(ANDROID_LOG_ERROR, "getPointPositions end", "%d, %d", 0, 0);

//홍채 원 구하기
//홍채 원 후보중에서 pointM과 가장 많이 겹치는 원을 홍채라 판단
            int idx = -1;   // 원 인덱스
            int maxn = 0;   //겹치는 최대 픽셀 수
            if (!candi_circles.empty()) {
                Mat black(contourM.rows, contourM.cols, contourM.type(), Scalar::all(0));

                for (int i = 0; i < candi_circles.size(); i++) {
                    if (pow(irisC_.x - candi_circles[i][0], 2) +
                        pow(irisC_.y - candi_circles[i][1], 2) < pow(candi_circles[i][2], 2)) {
                        black = Scalar::all(0);
                        circle(black, Point2d(candi_circles[i][0], candi_circles[i][1]),
                               (int) candi_circles[i][2], Scalar::all(255), 2);
                        bitwise_and(pointsM, black, black);
//int num = 0;
//                    for (int y = 0; y < black.rows; y++) {
//                        unsigned char *ptr = black.ptr<uchar>(y);
//                        for (int x = 0; x < black.cols; x++) {
//                            if (ptr[x] > 0) {
//                                num++;
//                            }
//                        }
//                    }
                        Mat numM;
                        findNonZero(black, numM);
                        int num = (int) numM.total();
                        if (num > maxn) {
                            maxn = num;
                            idx = i;
                        }
                    }
                }
            }


            Rect2f bf_iris_rect(rEyeCenter.x - 0.34f * rEyeRect.width,
                                rEyeCenter.y - 0.34f * rEyeRect.height, 0.68f * rEyeRect.width,
                                0.68f * rEyeRect.width);

            if (idx >= 0) {
//데이터 넘김
                ptrIrisCenter[0] = candi_circles[idx][0]; //x
                ptrIrisCenter[1] = candi_circles[idx][1]; //y
                ptrIrisCenter[2] = candi_circles[idx][2]; //r

                Point2f findCenter(rEyeRect.x + translateN(ptrIrisCenter[0], 200.f, rEyeRect.width),
                                   rEyeRect.y +
                                   translateN(ptrIrisCenter[1], 200.f, rEyeRect.width));
                float findRadius = translateN(ptrIrisCenter[2], 200.f, rEyeRect.width);

                bf_iris_rect.x = findCenter.x - findRadius * 1.25f;
                bf_iris_rect.y = findCenter.y - findRadius * 1.25f;
                bf_iris_rect.width = findRadius * 2.5f;
                bf_iris_rect.height = findRadius * 2.5f;

//processing OF
                Mat rIrisRectMat = matInput(bf_iris_rect).clone();

//-- 21. rIrisRectMat
//            resultMat = rIrisRectMat.clone();
//            try {
//                __android_log_print(ANDROID_LOG_ERROR, "buff_rIrisRectMat :0_", "%d, %d", 0, 0);
//                Mat buff_rIrisRectMat;
//                __android_log_print(ANDROID_LOG_ERROR, "buff_rIrisRectMat :1_", "%d, %d", 0, 0);
////                buff_rIrisRectMat = rIrisRectMat.clone();
//                flip(rIrisRectMat.clone(), buff_rIrisRectMat, 1);
//                __android_log_print(ANDROID_LOG_ERROR, "buff_rIrisRectMat :2_", "%d, %d", 0, 0);
//                char str_rIrisRectMat[] = "";
//                __android_log_print(ANDROID_LOG_ERROR, "buff_rIrisRectMat :3_", "%d, %d", 0, 0);
//                sprintf(str_rIrisRectMat, "%s_%d_%s",
//                        "/storage/emulated/0/Download/SDData/test/_21_", frameCnt_returnPupilSize,
//                        "_rIrisRectMat_.jpg");
//                __android_log_print(ANDROID_LOG_ERROR, "buff_rIrisRectMat :4_", "%d, %d", 0, 0);
//                cv::imwrite(str_rIrisRectMat, buff_rIrisRectMat);
//                __android_log_print(ANDROID_LOG_ERROR, "buff_rIrisRectMat :5_", "%d, %d", 0, 0);
//            } catch (const std::exception &e) {
//                __android_log_print(ANDROID_LOG_ERROR, "buff_rIrisRectMat :6_", "%d, %d", 0, 0);
//                __android_log_print(ANDROID_LOG_ERROR, "exception-buff_rIrisRectMat :", "%c, %d", e.what(),
//                                    0);
//                std::cerr << "exception: " << e.what() << std::endl;
//            }

//optical image view
                Mat grayopMat;
                Mat opMat;

                cvtColor(rIrisRectMat, grayopMat, COLOR_RGB2GRAY);
                cvtColor(grayopMat, opMat, COLOR_GRAY2RGB);

                Size opSize(91, 91);    // Optical Flow를 계산하기 위해 동일한 Mat 사이즈로 변환
                resize(opMat, opMat, opSize);

//            Mat buff_opMat;
//            buff_opMat = opMat.clone();
//            char str_opMat[] = "";
//            sprintf(str_opMat, "%s_%d_%s", "/storage/emulated/0/Download/SDData/test/_21_5_",
//                    frameCnt_returnPupilSize, "_opMat_.jpg");
//            cv::imwrite(str_opMat, buff_opMat);


                OFVars[0] = findCenter.x;   // PRY 홍채 추적 알고리즘
                OFVars[1] = findCenter.y;   // PRY 홍채 추적 알고리즘
                OFVars[2] = findRadius * 2;   // PRY 홍채 추적 알고리즘

// 2020.02.11   // 홍채 영역 그리기
//circle(resultMat, Point2f(OFVars[0], OFVars[1]), OFVars[2], Scalar(255, 0, 0), 1);

                OF_GAvg = optical_Flow(opMat);      // OF dist 데이터 --> 생체 정보 추론

//            resultMat = opMat.clone();

//            __android_log_print(ANDROID_LOG_ERROR, "-- 22. cflowF", "%d, %d", 0, 0);
//            if (frameCnt_returnPupilSize > 1) {
//-- 22. cflowF
//            resultMat = cflowF.clone();
//                try {
//                    __android_log_print(ANDROID_LOG_ERROR, "buff_cflowF :0_", "%d, %d", 0, 0);
//                    Mat buff_cflowF;
//                    __android_log_print(ANDROID_LOG_ERROR, "buff_cflowF :1_", "%d, %d", 0, 0);
////                    buff_cflowF = cflowF.clone();
//                    flip(cflowF.clone(), buff_cflowF, 1);
//                    __android_log_print(ANDROID_LOG_ERROR, "buff_cflowF :2_", "%d, %d", 0, 0);
//                    char str_cflowF[] = "";
//                    __android_log_print(ANDROID_LOG_ERROR, "buff_cflowF :3_", "%d, %d", 0, 0);
//                    sprintf(str_cflowF, "%s_%d_%s", "/storage/emulated/0/Download/SDData/test/_22_",
//                            frameCnt_returnPupilSize, "_cflowF_.jpg");
//                    __android_log_print(ANDROID_LOG_ERROR, "buff_cflowF :4_", "%d, %d", 0, 0);
//                    cv::imwrite(str_cflowF, buff_cflowF);
//                    __android_log_print(ANDROID_LOG_ERROR, "buff_cflowF :5_", "%d, %d", 0, 0);
//                } catch (const std::exception &e) {
//                    __android_log_print(ANDROID_LOG_ERROR, "buff_cflowF :6_", "%d, %d", 0, 0);
//                    __android_log_print(ANDROID_LOG_ERROR, "exception-buff_cflowF :", "%c, %d", e.what(),
//                                        0);
//                    std::cerr << "exception: " << e.what() << std::endl;
//                }
//            }
//            __android_log_print(ANDROID_LOG_ERROR, "-- 22. cflowF end", "%d, %d", 0, 0);

            }
            env->ReleaseFloatArrayElements(irisCenter, ptrIrisCenter, 0);

//  // android 버젼에는 없지만, OpenCV ROI 에러로 추가함 >> PRY 홍채 검출 알고리즘 대신 구글의 FACE DETECTION 알고리즘의 홍채 중심 좌표를 기준으로 영역을 가져오기에 if문으로 예외 처리 해줘야함
//        if (matInput.rows > 0 && (rIrisRect.x >= 0 && rIrisRect.x + rIrisRect.width < matInput.cols && rIrisRect.y >= 0 && rIrisRect.y + rIrisRect.height < matInput.rows )) {
//
//            Mat rIrisRectMat = matInput(rIrisRect).clone();
//
//            //optical image view
//            Mat grayopMat;
//            Mat opMat;
//
//            cvtColor(rIrisRectMat, grayopMat, COLOR_RGB2GRAY);
//            cvtColor(grayopMat, opMat, CV_GRAY2RGB);
//
//            Size opSize(91, 91);    // Optical Flow를 계산하기 위해 동일한 Mat 사이즈로 변환
//            resize(opMat, opMat, opSize);
//
//            OFVars[0] = ptrIrisCenter[0];   // PRY 홍채 추적 알고리즘
//            OFVars[1] = ptrIrisCenter[1];   // PRY 홍채 추적 알고리즘
//            OFVars[2] = ptrIrisCenter[2]/2;   // PRY 홍채 추적 알고리즘
//
//            // 2020.02.11   // 홍채 영역 그리기
//            circle(resultMat, Point2f(OFVars[0], OFVars[1]), OFVars[2], Scalar(255, 0, 0), 1);
//
//            OF_GAvg = optical_Flow(opMat);      // OF dist 데이터 --> 생체 정보 추론
//
//            //resultMat = opMat.clone();
//            resultMat = cflowF.clone();
//
//            //cout << "OF_GAvg: " << OF_GAvg << endl;
//            //__android_log_print(ANDROID_LOG_DEBUG, "OF_GAvg", "%lf \n", OF_GAvg);
//
//
//        }

        }

        //MARK: SP
        for (int i = 0; i < 244; i++) {
            ptrOFArry[i] = round(OFVars[i] * 1000000) / 1000000.0;
        }
        env->ReleaseFloatArrayElements(OF_arry, ptrOFArry, 0);
        env->ReleaseIntArrayElements(RGB_val, rgb_valArry, 0);
    } catch (Exception &e) {
        __android_log_print(ANDROID_LOG_DEBUG, "e.what()", "%s \n", e.what());

        return false;
    }
    return true;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_sd_cardivu_1dev_1beta_MeasureActivity_setSersorLux(JNIEnv *env, jobject thiz,
                                                            jfloat sersor__lux) {
// TODO: implement setSersorLux()
    sersor_Lux = sersor__lux;

}

double optical_Flow(Mat &frame) {
    Mat frame_1Q, frame_2Q, frame_3Q, frame_4Q, frame_Avg, frame_34Avg;     // LDW 191106
    Mat fframe_1Q, fframe_2Q, fframe_3Q;        // LDW 191106
    Mat fframe_34Avg;        // LDW 191106

    Rect rect_1Q(frame.rows / 2, 0, frame.rows / 2, frame.rows / 2),
            rect_2Q(0, 0, frame.rows / 2, frame.rows / 2),
            rect_3Q(0, frame.rows / 2, frame.rows / 2, frame.rows / 2),
            rect_4Q(frame.rows / 2, frame.rows / 2, frame.rows / 2, frame.rows / 2);

    //데이터 확인
//    __android_log_print(ANDROID_LOG_ERROR, "opticalFlow\trect_1Q\t", "%d\t%d\t%d\t%d",
//                        frame.rows / 2, 0, frame.rows / 2, frame.rows / 2);
//    __android_log_print(ANDROID_LOG_ERROR, "opticalFlow\trect_2Q\t", "%d\t%d\t%d\t%d", 0, 0,
//                        frame.rows / 2, frame.rows / 2);
//    __android_log_print(ANDROID_LOG_ERROR, "opticalFlow\trect_3Q\t", "%d\t%d\t%d\t%d", 0,
//                        frame.rows / 2, frame.rows / 2, frame.rows / 2);
//    __android_log_print(ANDROID_LOG_ERROR, "opticalFlow\trect_4Q\t", "%d\t%d\t%d\t%d",
//                        frame.rows / 2, frame.rows / 2, frame.rows / 2, frame.rows / 2);

    frame_1Q = frame(rect_1Q);
    frame_2Q = frame(rect_2Q);
    frame_3Q = frame(rect_3Q);
    frame_4Q = frame(rect_4Q);

    flip(frame_1Q, fframe_1Q, 0);        // LDW 191106
    flip(frame_2Q, fframe_2Q, -1);        // LDW 191106
    flip(frame_3Q, fframe_3Q, 1);        // LDW 191106

//    frame_Avg = fframe_1Q / 4 + fframe_2Q / 4 + fframe_3Q / 4 + frame_4Q / 4;        // LDW 191106
//    frame_34Avg = fframe_3Q / 2 + frame_4Q / 2;        // LDW 191106
//    //imshow("frame_Avg", frame_Avg);
//
//    Mat pre_buff_frame_34Avg;
//    pre_buff_frame_34Avg = frame_34Avg.clone();
//    char pre_str_frame_34Avg[] = "";
//    sprintf(pre_str_frame_34Avg, "%s_%d_%s", "/storage/emulated/0/Download/test/",
//            frameCnt_returnPupilSize, "_pre_frame_34Avg_.jpg");
//    cv::imwrite(pre_str_frame_34Avg, pre_buff_frame_34Avg);
//
//    Mat pre_buff_frame_Avg;
//    pre_buff_frame_Avg = frame_Avg.clone();
//    char pre_str_frame_Avg[] = "";
//    sprintf(pre_str_frame_Avg, "%s_%d_%s", "/storage/emulated/0/Download/test/",
//            frameCnt_returnPupilSize, "_pre_frame_Avg_.jpg");
//    cv::imwrite(pre_str_frame_Avg, pre_buff_frame_Avg);


    // 20200703 RY 수정
    Mat fframe12Q;
    addWeighted(fframe_1Q, 0.5, fframe_2Q, 0.5, 0, fframe12Q);
    addWeighted(fframe_3Q, 0.5, frame_4Q, 0.5, 0, frame_34Avg);
    addWeighted(fframe12Q, 0.5, frame_34Avg, 0.5, 0, frame_Avg);

//    Mat buff_frame_34Avg;
//    buff_frame_34Avg = frame_34Avg.clone();
//    char str_frame_34Avg[] = "";
//    sprintf(str_frame_34Avg, "%s_%d_%s", "/storage/emulated/0/Download/test/",
//            frameCnt_returnPupilSize, "_frame_34Avg_.jpg");
//    cv::imwrite(str_frame_34Avg, buff_frame_34Avg);
//
//    Mat buff_frame_Avg;
//    buff_frame_Avg = frame_Avg.clone();
//    char str_frame_Avg[] = "";
//    sprintf(str_frame_Avg, "%s_%d_%s", "/storage/emulated/0/Download/test/",
//            frameCnt_returnPupilSize, "_frame_Avg_.jpg");
//    cv::imwrite(str_frame_Avg, buff_frame_Avg);

    cvtColor(fframe_1Q, gray_1Q, COLOR_RGB2GRAY);        // LDW 191106
    cvtColor(fframe_2Q, gray_2Q, COLOR_RGB2GRAY);        // LDW 191106
    cvtColor(fframe_3Q, gray_3Q, COLOR_RGB2GRAY);        // LDW 191106
    cvtColor(frame_4Q, gray_4Q, COLOR_RGB2GRAY);        // LDW 191106
    cvtColor(frame_Avg, gray_Avg, COLOR_RGB2GRAY);        // LDW 191106
    cvtColor(frame_34Avg, gray_34Avg, COLOR_RGB2GRAY);        // LDW 191106
    cvtColor(frame, grayF, COLOR_RGB2GRAY);

    //데이터 확인 fframe_1Q
//    Mat buff_fframe_1Q;
//    buff_fframe_1Q = fframe_1Q.clone();
//    cvtColor(buff_fframe_1Q, buff_fframe_1Q, COLOR_RGB2GRAY);
//    char str_fframe_1Q[] = "";
//    sprintf(str_fframe_1Q, "%s_%d_%s", "/storage/emulated/0/Download/SDData/test/_1_",
//            frameCnt_returnPupilSize, "_fframe_1Q_.jpg");
//    cv::imwrite(str_fframe_1Q, buff_fframe_1Q);

    //데이터 확인 fframe_2Q
//    Mat buff_fframe_2Q;
//    buff_fframe_2Q = fframe_2Q.clone();
//    cvtColor(buff_fframe_2Q, buff_fframe_2Q, COLOR_RGB2GRAY);
//    char str_fframe_2Q[] = "";
//    sprintf(str_fframe_2Q, "%s_%d_%s", "/storage/emulated/0/Download/SDData/test/_2_",
//            frameCnt_returnPupilSize, "_fframe_2Q_.jpg");
//    cv::imwrite(str_fframe_2Q, buff_fframe_2Q);

    //데이터 확인 fframe_3Q
//    Mat buff_fframe_3Q;
//    buff_fframe_3Q = fframe_3Q.clone();
//    cvtColor(buff_fframe_3Q, buff_fframe_3Q, COLOR_RGB2GRAY);
//    char str_fframe_3Q[] = "";
//    sprintf(str_fframe_3Q, "%s_%d_%s", "/storage/emulated/0/Download/SDData/test/_3_",
//            frameCnt_returnPupilSize, "_fframe_3Q_.jpg");
//    cv::imwrite(str_fframe_3Q, buff_fframe_3Q);

    //데이터 확인 frame_4Q
//    Mat buff_fframe_4Q;
//    buff_fframe_4Q = frame_4Q.clone();
//    cvtColor(buff_fframe_4Q, buff_fframe_4Q, COLOR_RGB2GRAY);
//    char str_fframe_4Q[] = "";
//    sprintf(str_fframe_4Q, "%s_%d_%s", "/storage/emulated/0/Download/SDData/test/_4_",
//            frameCnt_returnPupilSize, "_fframe_4Q_.jpg");
//    cv::imwrite(str_fframe_4Q, buff_fframe_2Q);

    //데이터 확인 frame_Avg
//    Mat buff_frame_Avg;
//    buff_frame_Avg = frame_Avg.clone();
//    cvtColor(buff_frame_Avg, buff_frame_Avg, COLOR_RGB2GRAY);
//    char str_frame_Avg[] = "";
//    sprintf(str_frame_Avg, "%s_%d_%s", "/storage/emulated/0/Download/SDData/test/_5_",
//            frameCnt_returnPupilSize, "_frame_Avg_.jpg");
//    cv::imwrite(str_frame_Avg, buff_frame_Avg);

    //데이터 확인 frame_34Avg
//    Mat buff_frame_34Avg;
//    buff_frame_34Avg = frame_34Avg.clone();
//    cvtColor(buff_frame_34Avg, buff_frame_34Avg, COLOR_RGB2GRAY);
//    char str_frame_34Avg[] = "";
//    sprintf(str_frame_34Avg, "%s_%d_%s", "/storage/emulated/0/Download/SDData/test/_6_",
//            frameCnt_returnPupilSize, "_frame_34Avg_.jpg");
//    cv::imwrite(str_frame_34Avg, buff_frame_34Avg);

    //데이터 확인 frame
//    Mat buff_frame;
//    buff_frame = frame.clone();
//    cvtColor(buff_frame, buff_frame, COLOR_RGB2GRAY);
//    char str_frame[] = "";
//    sprintf(str_frame, "%s_%d_%s", "/storage/emulated/0/Download/SDData/test/_0_",
//            frameCnt_returnPupilSize, "_frame_.jpg");
//    cv::imwrite(str_frame, buff_frame);

    ////////////////////////////        // LDW 191106       /////////////////////////////////////////

    if (!prevgrayF.empty()) {       // FULL IRIS
        calcOpticalFlowFarneback(prevgrayF, grayF, uflowF, 0.5, 3, 6, 6, 5, 1.2, 0);
        // pyramid scale: 0.5, levels: 3, window size(비교 window size): 15, iteration(민감도, 값이 커지면 frame rate 저하): 3, poly_n: 5, poly_sigma: 1.2
        cvtColor(prevgrayF, cflowF, COLOR_GRAY2RGB);
        uflowF.copyTo(flowF);
        drawOptFlowMap(flowF, cflowF, 6, 2, Scalar(200, 0, 0), 0);
        // step (간격): 10
        //imshow("flow_Avg", cflow_Avg);
    }
    swap(prevgrayF, grayF);

    if (!prevgray_1Q.empty())       // 1Q IRIS
    {
        calcOpticalFlowFarneback(prevgray_1Q, gray_1Q, uflow_1Q, 0.5, 3, 6, 6, 5, 1.2, 0);
        // pyramid scale: 0.5, levels: 3, window size(비교 window size): 15, iteration(민감도, 값이 커지면 frame rate 저하): 3, poly_n: 5, poly_sigma: 1.2
        cvtColor(prevgray_1Q, cflow_1Q, COLOR_GRAY2RGB);
        uflow_1Q.copyTo(flow_1Q);
        drawOptFlowMap(flow_1Q, cflow_1Q, 6, 2, Scalar(200, 0, 0), 1);
        // step (간격): 10
        //imshow("flow_1Q", cflow_1Q);
    }
    swap(prevgray_1Q, gray_1Q);

    if (!prevgray_2Q.empty())       // 2Q IRIS
    {
        calcOpticalFlowFarneback(prevgray_2Q, gray_2Q, uflow_2Q, 0.5, 3, 6, 6, 5, 1.2, 0);
        // pyramid scale: 0.5, levels: 3, window size(비교 window size): 15, iteration(민감도, 값이 커지면 frame rate 저하): 3, poly_n: 5, poly_sigma: 1.2
        cvtColor(prevgray_2Q, cflow_2Q, COLOR_GRAY2RGB);
        uflow_2Q.copyTo(flow_2Q);
        drawOptFlowMap(flow_2Q, cflow_2Q, 6, 2, Scalar(200, 0, 0), 2);
        // step (간격): 10
        //imshow("flow_2Q", cflow_2Q);
    }
    swap(prevgray_2Q, gray_2Q);

    if (!prevgray_3Q.empty())       // 3Q IRIS
    {
        calcOpticalFlowFarneback(prevgray_3Q, gray_3Q, uflow_3Q, 0.5, 3, 6, 6, 5, 1.2, 0);
        // pyramid scale: 0.5, levels: 3, window size(비교 window size): 15, iteration(민감도, 값이 커지면 frame rate 저하): 3, poly_n: 5, poly_sigma: 1.2
        cvtColor(prevgray_3Q, cflow_3Q, COLOR_GRAY2RGB);
        uflow_3Q.copyTo(flow_3Q);
        drawOptFlowMap(flow_3Q, cflow_3Q, 6, 2, Scalar(200, 0, 0), 3);
        // step (간격): 10
        //imshow("flow_3Q", cflow_3Q);
    }
    swap(prevgray_3Q, gray_3Q);

    if (!prevgray_4Q.empty())       // 4Q IRIS
    {
        calcOpticalFlowFarneback(prevgray_4Q, gray_4Q, uflow_4Q, 0.5, 3, 6, 6, 5, 1.2, 0);
        // pyramid scale: 0.5, levels: 3, window size(비교 window size): 15, iteration(민감도, 값이 커지면 frame rate 저하): 3, poly_n: 5, poly_sigma: 1.2
        cvtColor(prevgray_4Q, cflow_4Q, COLOR_GRAY2RGB);
        uflow_4Q.copyTo(flow_4Q);
        drawOptFlowMap(flow_4Q, cflow_4Q, 6, 2, Scalar(200, 0, 0), 4);
        // step (간격): 10
        //imshow("flow_4Q", cflow_4Q);
    }
    swap(prevgray_4Q, gray_4Q);

    if (!prevgray_Avg.empty()) {        // GAVG IRIS
        //calcOpticalFlowFarneback(prevgray_Avg, gray_Avg, uflow_Avg, 0.5, 3, 3, 10, 5, 1.2, 0);
        calcOpticalFlowFarneback(prevgray_Avg, gray_Avg, uflow_Avg, 0.5, 3, 6, 6, 5, 1.2, 0);
        // pyramid scale: 0.5, levels: 3, window size(비교 window size): 15, iteration(민감도, 값이 커지면 frame rate 저하): 3, poly_n: 5, poly_sigma: 1.2
        cvtColor(prevgray_Avg, cflow_Avg, COLOR_GRAY2RGB);
        uflow_Avg.copyTo(flow_Avg);

        drawOptFlowMap(flow_Avg, cflow_Avg, 6, 2, Scalar(200, 0, 0), 5);

//        __android_log_print(ANDROID_LOG_DEBUG, "GAVGGAVGGAVGGAVGGAVGGAVG IRIS: ", "%lf\n", OF_GAvg);

        // step (간격): 10
        //imshow("flow_Avg", cflow_Avg);
    }
    swap(prevgray_Avg, gray_Avg);

    if (!prevgray_34Avg.empty()) {
        calcOpticalFlowFarneback(prevgray_34Avg, gray_34Avg, uflow_34Avg, 0.5, 3, 6, 6, 5, 1.2, 0);
        // pyramid scale: 0.5, levels: 3, window size(비교 window size): 15, iteration(민감도, 값이 커지면 frame rate 저하): 3, poly_n: 5, poly_sigma: 1.2
        cvtColor(prevgray_34Avg, cflow_34Avg, COLOR_GRAY2RGB);
        uflow_34Avg.copyTo(flow_34Avg);
        OF_GAvg = drawOptFlowMap(flow_34Avg, cflow_34Avg, 6, 2, Scalar(200, 0, 0), 6);
        // step (간격): 10
        //imshow("flow_34Avg", cflow_34Avg);
    }
    swap(prevgray_34Avg, gray_34Avg);

    ////////////////////////////        // LDW 191106       /////////////////////////////////////////

    return OF_GAvg;
}

double drawOptFlowMap(const Mat &flow, Mat &cflowmap, int step, double, const Scalar &color,
                      int labelNum) {
    double sum_dist = 0.0;

    ////////////////////////////        // LDW 191106       /////////////////////////////////////////

    double sum_InC = 0.0, sum_DeC = 0.0, sum_1InC = 0.0, sum_1DeC = 0.0, sum_2InC = 0.0, sum_2DeC = 0.0, sum_3InC = 0.0, sum_3DeC = 0.0, sum_4InC = 0.0, sum_4DeC = 0.0;
    int cnt_InC = 0, cnt_DeC = 0, cnt_Total = 0, cnt_1InC = 0, cnt_1DeC = 0, cnt_1Total = 0, cnt_2InC = 0, cnt_2DeC = 0, cnt_2Total = 0, cnt_3InC = 0, cnt_3DeC = 0, cnt_3Total = 0, cnt_4InC = 0, cnt_4DeC = 0, cnt_4Total = 0;

    double sum_Rearr = 0.0, sum_1Rearr = 0.0, sum_2Rearr = 0.0, sum_3Rearr = 0.0, sum_4Rearr = 0.0;

    double sum_1dist = 0.0; // about pupil
    double sum_2dist = 0.0; // about Collarette (frill)
    double sum_3dist = 0.0; // about iris
    double sum_4dist = 0.0; // about rest

    if (labelNum == 0) {
        for (int y = 0; y < cflowmap.rows; y += step) {
            for (int x = 0; x < cflowmap.cols; x += step) {
                const Point2f &fxy = flow.at<Point2f>(y, x);
                sum_dist += sqrt(fxy.x * fxy.x + fxy.y * fxy.y);

                //line(cflowmap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)), color);
                //circle(cflowmap, Point(x, y), 2, color, -1);
            }
        }
        //__android_log_print(ANDROID_LOG_DEBUG, "sum_dist: ", "%lf \t", sum_dist);
        OFVars[3] = sum_dist;
    } else {

        for (int y = 0; y < cflowmap.rows; y += step)
            for (int x = 0; x < cflowmap.cols; x += step) {
                const Point2f &fxy = flow.at<Point2f>(y, x);

                if (sqrt((x * x) + (y * y)) <= 15)    // about pupil
                {
                    //line(cflowmap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)), Scalar(200, 200, 200));

                    sum_1dist += sqrt(fxy.x * fxy.x + fxy.y * fxy.y);
                    //circle(cflowmap, Point(x, y), 2, Scalar(200, 200, 200), -1);

                    if (fxy.x >= 0 && fxy.y >= 0)    // 확장
                    {
                        sum_1InC += sqrt(fxy.x * fxy.x + fxy.y * fxy.y);
                        cnt_1InC++;
                    } else if (fxy.x < 0 && fxy.y < 0)    // 축소
                    {
                        sum_1DeC += sqrt(fxy.x * fxy.x + fxy.y * fxy.y);
                        cnt_1DeC++;
                    }
                    cnt_1Total++;
                } else if (sqrt((x * x) + (y * y)) <= 25)    // about Collarette (frill)
                {
                    //line(cflowmap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)), Scalar(0, 0, 200));

                    sum_2dist += sqrt(fxy.x * fxy.x + fxy.y * fxy.y);
                    //circle(cflowmap, Point(x, y), 2, Scalar(0, 0, 200), -1);

                    if (fxy.x >= 0 && fxy.y >= 0)    // 확장
                    {
                        sum_2InC += sqrt(fxy.x * fxy.x + fxy.y * fxy.y);
                        cnt_2InC++;
                    } else if (fxy.x < 0 && fxy.y < 0)    // 축소
                    {
                        sum_2DeC += sqrt(fxy.x * fxy.x + fxy.y * fxy.y);
                        cnt_2DeC++;
                    }
                    cnt_2Total++;
                } else if (sqrt((x * x) + (y * y)) <= 45)    // about iris
                {
                    //line(cflowmap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)), Scalar(0, 200, 0));

                    sum_3dist += sqrt(fxy.x * fxy.x + fxy.y * fxy.y);
                    //circle(cflowmap, Point(x, y), 2, Scalar(0, 200, 0), -1);

                    if (fxy.x >= 0 && fxy.y >= 0)    // 확장
                    {
                        sum_3InC += sqrt(fxy.x * fxy.x + fxy.y * fxy.y);
                        cnt_3InC++;
                    } else if (fxy.x < 0 && fxy.y < 0)    // 축소
                    {
                        sum_3DeC += sqrt(fxy.x * fxy.x + fxy.y * fxy.y);
                        cnt_3DeC++;
                    }
                    cnt_3Total++;
                } else {
                    //line(cflowmap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)), Scalar(200, 0, 0));

                    sum_4dist += sqrt(fxy.x * fxy.x + fxy.y * fxy.y);
                    //circle(cflowmap, Point(x, y), 2, Scalar(200, 0, 0), -1);

                    if (fxy.x >= 0 && fxy.y >= 0)    // 확장
                    {
                        sum_4InC += sqrt(fxy.x * fxy.x + fxy.y * fxy.y);
                        cnt_4InC++;
                    } else if (fxy.x < 0 && fxy.y < 0)    // 축소
                    {
                        sum_4DeC += sqrt(fxy.x * fxy.x + fxy.y * fxy.y);
                        cnt_4DeC++;
                    }
                    cnt_4Total++;
                }

                cnt_Total++;
            }

        sum_dist = sum_1dist + sum_2dist + sum_3dist + sum_4dist;
        sum_InC = sum_1InC + sum_2InC + sum_3InC + sum_4InC;
        cnt_InC = cnt_1InC + cnt_2InC + cnt_3InC + cnt_4InC;
        sum_DeC = sum_1DeC + sum_2DeC + sum_3DeC + sum_4DeC;
        cnt_DeC = cnt_1DeC + cnt_2DeC + cnt_3DeC + cnt_4DeC;


        if (((double) cnt_InC / cnt_Total) >= ((double) cnt_DeC / cnt_Total))
            sum_Rearr = sum_InC;
        else
            sum_Rearr = -sum_DeC;

        if (((double) cnt_1InC / cnt_1Total) >= ((double) cnt_1DeC / cnt_1Total))
            sum_1Rearr = sum_1InC;
        else
            sum_1Rearr = -sum_1DeC;

        if (((double) cnt_2InC / cnt_2Total) >= ((double) cnt_2DeC / cnt_2Total))
            sum_2Rearr = sum_2InC;
        else
            sum_2Rearr = -sum_2DeC;

        if (((double) cnt_3InC / cnt_3Total) >= ((double) cnt_3DeC / cnt_3Total))
            sum_3Rearr = sum_3InC;
        else
            sum_3Rearr = -sum_3DeC;

        if (((double) cnt_4InC / cnt_4Total) >= ((double) cnt_4DeC / cnt_4Total))
            sum_4Rearr = sum_4InC;
        else
            sum_4Rearr = -sum_4DeC;

        OFVars[4 + ((labelNum - 1) * 40)] = sum_dist;
        OFVars[5 + ((labelNum - 1) * 40)] = sum_InC;
        OFVars[6 + ((labelNum - 1) * 40)] = sum_DeC;
        OFVars[7 + ((labelNum - 1) * 40)] = ((double) cnt_InC / cnt_Total) * 100;
        OFVars[8 + ((labelNum - 1) * 40)] = ((double) cnt_DeC / cnt_Total) * 100;
        OFVars[9 + ((labelNum - 1) * 40)] = ((double) (sum_InC - sum_DeC));
        OFVars[10 + ((labelNum - 1) * 40)] = sum_Rearr;
        OFVars[11 + ((labelNum - 1) * 40)] = ((sum_InC * ((double) cnt_InC / cnt_Total)) -
                                              (sum_DeC * ((double) cnt_DeC / cnt_Total)));

        OFVars[12 + ((labelNum - 1) * 40)] = sum_1dist;
        OFVars[13 + ((labelNum - 1) * 40)] = sum_1InC;
        OFVars[14 + ((labelNum - 1) * 40)] = sum_1DeC;
        OFVars[15 + ((labelNum - 1) * 40)] = ((double) cnt_1InC / cnt_1Total) * 100;
        OFVars[16 + ((labelNum - 1) * 40)] = ((double) cnt_1DeC / cnt_1Total) * 100;
        OFVars[17 + ((labelNum - 1) * 40)] = ((double) (sum_1InC - sum_1DeC));
        OFVars[18 + ((labelNum - 1) * 40)] = sum_1Rearr;
        OFVars[19 + ((labelNum - 1) * 40)] = ((sum_1InC * ((double) cnt_1InC / cnt_1Total)) -
                                              (sum_1DeC * ((double) cnt_1DeC / cnt_1Total)));

        OFVars[20 + ((labelNum - 1) * 40)] = sum_2dist;
        OFVars[21 + ((labelNum - 1) * 40)] = sum_2InC;
        OFVars[22 + ((labelNum - 1) * 40)] = sum_2DeC;
        OFVars[23 + ((labelNum - 1) * 40)] = ((double) cnt_2InC / cnt_2Total) * 100;
        OFVars[24 + ((labelNum - 1) * 40)] = ((double) cnt_2DeC / cnt_2Total) * 100;
        OFVars[25 + ((labelNum - 1) * 40)] = ((double) (sum_2InC - sum_2DeC));
        OFVars[26 + ((labelNum - 1) * 40)] = sum_2Rearr;
        OFVars[27 + ((labelNum - 1) * 40)] = ((sum_2InC * ((double) cnt_2InC / cnt_2Total)) -
                                              (sum_2DeC * ((double) cnt_2DeC / cnt_2Total)));

        OFVars[28 + ((labelNum - 1) * 40)] = sum_3dist;
        OFVars[29 + ((labelNum - 1) * 40)] = sum_3InC;
        OFVars[30 + ((labelNum - 1) * 40)] = sum_3DeC;
        OFVars[31 + ((labelNum - 1) * 40)] = ((double) cnt_3InC / cnt_3Total) * 100;
        OFVars[32 + ((labelNum - 1) * 40)] = ((double) cnt_3DeC / cnt_3Total) * 100;
        OFVars[33 + ((labelNum - 1) * 40)] = ((double) (sum_3InC - sum_3DeC));
        OFVars[34 + ((labelNum - 1) * 40)] = sum_3Rearr;
        OFVars[35 + ((labelNum - 1) * 40)] = ((sum_3InC * ((double) cnt_3InC / cnt_3Total)) -
                                              (sum_3DeC * ((double) cnt_3DeC / cnt_3Total)));

        OFVars[36 + ((labelNum - 1) * 40)] = sum_4dist;
        OFVars[37 + ((labelNum - 1) * 40)] = sum_4InC;
        OFVars[38 + ((labelNum - 1) * 40)] = sum_4DeC;
        OFVars[39 + ((labelNum - 1) * 40)] = ((double) cnt_4InC / cnt_4Total) * 100;
        OFVars[40 + ((labelNum - 1) * 40)] = ((double) cnt_4DeC / cnt_4Total) * 100;
        OFVars[41 + ((labelNum - 1) * 40)] = ((double) (sum_4InC - sum_4DeC));
        OFVars[42 + ((labelNum - 1) * 40)] = sum_4Rearr;
        OFVars[43 + ((labelNum - 1) * 40)] = ((sum_4InC * ((double) cnt_4InC / cnt_4Total)) -
                                              (sum_4DeC * ((double) cnt_4DeC / cnt_4Total)));

//
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_dist: ", "%lf \t", sum_dist);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_InC: ", "%lf \t", sum_InC);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_DeC: ", "%lf \t", sum_DeC);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_pIeC: ", "%lf \t", ((double)cnt_InC / cnt_Total) * 100);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_pDeC: ", "%lf \t", ((double)cnt_DeC / cnt_Total) * 100);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_InC-DeC: ", "%lf \t", ((double)(sum_InC - sum_DeC)));
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_InC/DeC: ", "%lf \t", sum_Rearr);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_InCp-DeCp: ", "%lf \n", ((sum_InC * ((double)cnt_InC / cnt_Total)) - (sum_DeC * ((double)cnt_DeC / cnt_Total))));
//
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_1dist: ", "%lf \t", sum_1dist);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_1InC: ", "%lf \t", sum_1InC);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_1DeC: ", "%lf \t", sum_1DeC);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_1pIeC: ", "%lf \t", ((double)cnt_1InC / cnt_1Total) * 100);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_1pDeC: ", "%lf \t", ((double)cnt_1DeC / cnt_1Total) * 100);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_1InC-DeC: ", "%lf \t", ((double)(sum_1InC - sum_1DeC)));
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_1InC/DeC: ", "%lf \t", sum_1Rearr);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_1InCp-DeCp: ", "%lf \n", ((sum_1InC * ((double)cnt_1InC / cnt_1Total)) - (sum_1DeC * ((double)cnt_1DeC / cnt_1Total))));
//
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_2dist: ", "%lf \t", sum_2dist);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_2InC: ", "%lf \t", sum_2InC);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_2DeC: ", "%lf \t", sum_2DeC);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_2pIeC: ", "%lf \t", ((double)cnt_2InC / cnt_2Total) * 100);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_2pDeC: ", "%lf \t", ((double)cnt_2DeC / cnt_2Total) * 100);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_2InC-DeC: ", "%lf \t", ((double)(sum_2InC - sum_2DeC)));
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_2InC/DeC: ", "%lf \t", sum_2Rearr);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_2InCp-DeCp: ", "%lf \n", ((sum_2InC * ((double)cnt_2InC / cnt_2Total)) - (sum_2DeC * ((double)cnt_2DeC / cnt_2Total))));
//
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_3dist: ", "%lf \t", sum_3dist);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_3InC: ", "%lf \t", sum_3InC);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_3DeC: ", "%lf \t", sum_3DeC);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_3pIeC: ", "%lf \t", ((double)cnt_3InC / cnt_3Total) * 100);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_3pDeC: ", "%lf \t", ((double)cnt_3DeC / cnt_3Total) * 100);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_3InC-DeC: ", "%lf \t", ((double)(sum_3InC - sum_3DeC)));
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_3InC/DeC: ", "%lf \t", sum_3Rearr);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_3InCp-DeCp: ", "%lf \n", ((sum_3InC * ((double)cnt_3InC / cnt_3Total)) - (sum_3DeC * ((double)cnt_3DeC / cnt_3Total))));
//
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_4dist: ", "%lf \t", sum_4dist);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_4InC: ", "%lf \t", sum_4InC);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_4DeC: ", "%lf \t", sum_4DeC);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_4pIeC: ", "%lf \t", ((double)cnt_4InC / cnt_4Total) * 100);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_4pDeC: ", "%lf \t", ((double)cnt_4DeC / cnt_4Total) * 100);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_4InC-DeC: ", "%lf \t", ((double)(sum_4InC - sum_4DeC)));
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_4InC/DeC: ", "%lf \t", sum_4Rearr);
//        __android_log_print(ANDROID_LOG_DEBUG, "sum_4InCp-DeCp: ", "%lf \n", ((sum_4InC * ((double)cnt_4InC / cnt_4Total)) - (sum_4DeC * ((double)cnt_4DeC / cnt_4Total))));

    }

    ////////////////////////////        // LDW 191106       /////////////////////////////////////////

    return sum_dist;
}

Mat getHistogram(const Mat &src, double *minVal, double *maxVal) {
    int histSize = 255;
    float hranges[] = {0.0, 255.0};
    const float *ranges = hranges;;
    const int *channels = {0};
    Mat hist;
    calcHist(&src, 1, channels, Mat(), hist, 1, &histSize, &ranges);
    minMaxLoc(hist, minVal, maxVal);
    return hist;
}

void thresVertical(Mat src, Mat &dst, int thres) {//이진화 결과에서 세로 경계를 얻음. src는 gray 영상
    threshold(src, img_thres, thres, 255, THRESH_BINARY_INV);
    morphologyEx(img_thres, dst, MORPH_ERODE, getStructuringElement(MORPH_ELLIPSE, Size(7, 3)));
    dst = img_thres - dst;
    morphologyEx(dst, dst, MORPH_OPEN, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));
    morphologyEx(dst, dst, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));
}

float translateN(float n, float src_len, float dst_len) {
    return n * (dst_len / src_len);
}

bool drawCirlceWith3P(Mat dst, Point p1, Point p2, Point p3, Point2d &center,
                      double &radius) {//입력된 3점으로 원을 그림. 그릴수 있으면 true, 원의 중심, 원의 반지름 return. 아니면 false return.
    //점의 좌표를 입력
    double x1 = p1.x;
    double y1 = p1.y;
    double x2 = p2.x;
    double y2 = p2.y;
    double x3 = p3.x;
    double y3 = p3.y;

    //두 수직 이등분선의 기울기를 입력
    double d1 = (x2 - x1) / (y2 - y1);
    double d2 = (x3 - x2) / (y3 - y2);

    if (isnan(d1))d1 = 0;
    if (isnan(d2))d2 = 0;

    //원의 중점을 구함
    double cx = ((y3 - y1) + (x2 + x3) * d2 - (x1 + x2) * d1) / (2 * (d2 - d1));
    double cy = -d1 * (cx - (x1 + x2) / 2) + (y1 + y2) / 2;

    if (!(isinf(cx) || isinf(cy) || isnan(cx) || isnan(cy))) {
        double r = sqrt(pow(x1 - cx, 2) + pow(y1 - cy, 2));
        if (r > 14 && r < dst.rows * 0.6 //홍채 사이즈 부분
            && cx + r < dst.cols && cx - r >= 0 && cy + r < dst.rows && cy - r >= 0
                ) {//원 범위 부분
            //circle(dst, Point(cx, cy), r, Scalar(100, 100, 100), 1);
            //circle(dst, Point(cx, cy), 2, Scalar(100, 100, 100), -1);
            center.x = cx;
            center.y = cy;
            radius = r;
            return true;
        }
    }
    return false;
}

void cannyVertical(Mat src, Mat &dst, int threshold1, int threshold2) {//canny edge 결과에서 세로 경계를 얻음
    Mat cany;
    Mat ttt;
    threshold(src, ttt, 200, 255, THRESH_BINARY);
    morphologyEx(ttt, ttt, MORPH_DILATE, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    Canny(src, cany, threshold1, threshold2);
    //cany -= ttt;
    morphologyEx(cany, dst, MORPH_DILATE, getStructuringElement(MORPH_ELLIPSE, Size(5, 3)));
    morphologyEx(dst, dst, MORPH_OPEN, getStructuringElement(MORPH_ELLIPSE, Size(1, 5)));
    morphologyEx(dst, dst, MORPH_DILATE, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    bitwise_and(dst, cany, dst);
    morphologyEx(dst, dst, MORPH_CLOSE, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    //dst=cany;
}

bool inCircle(Vec3d circle1, Vec3d circle2) {
    return pow(circle1[0] - circle2[0], 2) + pow(circle1[1] - circle2[1], 2) <
           pow(min(circle1[2], circle2[2]), 2.1);
}

void detectIris(Mat gray, Mat &dst, int mode) {
    Mat aaa;
    Mat vedge;
    std::vector<std::vector<Point>> select, candies, ct;
    std::vector<Vec3d> circles;

    if (mode == 0) {
        thresVertical(gray, vedge, 100);
        findContours(vedge, ct, RETR_EXTERNAL, CHAIN_APPROX_NONE);
        for (int i = 0; i < ct.size(); i++) {
            if (ct[i].size() < 50) {
                drawContours(vedge, ct, i, Scalar::all(0), -1);
            } else if (boundingRect(ct[i]).height > 20) {
                select.push_back(ct[i]);
            } else {
                drawContours(vedge, ct, i, Scalar::all(0), -1);
            }
        }
        //////////////imshow("v", vedge);

        if (select.size() > 0) {
            double num = 0;
            int fixels = vedge.cols * vedge.rows;

            /*float rr;
            Point2f cc;*/
            Mat mask, mask1, mask2;
            findp = false;
            drawp = false;
            for (int i = 0; i < select.size() - 1; i++) {
                Rect rt1, rt2;
                rt1 = boundingRect(select[i]);
                for (int j = i + 1; j < select.size(); j++) {
                    rt2 = boundingRect(select[j]);
                    if (abs(rt1.x - (rt2.x + rt2.width)) < vedge.rows / 2 &&
                        abs(rt1.y - (rt2.y + rt2.height)) < vedge.rows / 2) {
                        std::vector<Point> temp;
                        temp.insert(temp.begin(), select[i].begin(), select[i].end());
                        temp.insert(temp.end(), select[j].begin(), select[j].end());
                        Point2f c;
                        float r;
                        minEnclosingCircle(temp, c, r);
                        if (c.x + r < vedge.cols && c.x - r >= 0 &&
                            c.y + r < vedge.rows && c.y - r >= 0 && r < vedge.rows / 2) {
                            findp = true;
                            //circle(tempmat, c, r, Scalar::all(150));
                            //circle(img_original, c, r, Scalar(155, 150, 255));
                            if (select.size() > 2) {
                                mask1 = Mat::zeros(vedge.size(), vedge.type());
                                mask2 = mask1.clone();
                                circle(mask1, c, r, Scalar(255, 255, 255), 10);
                                drawContours(mask2, Mat(temp), -1, Scalar::all(255), -1);
                                bitwise_and(mask1, mask2, mask);
                                double minVal, maxVal;
                                Mat hist = getHistogram(mask, &minVal, &maxVal);
                                double cir_ratio = (fixels - maxVal) / (2.0 * r * CV_PI);
                                if (num < cir_ratio) {
                                    num = cir_ratio;
                                    rr = r;
                                    cc = c;
                                }
                            } else if (select.size() == 2) {
                                rr = r;
                                cc = c;
                            } else {
                                findp = false;

                            }
                        }
                    }
                }
            }

            if (findp) circle(dst, cc, rr, Scalar(0, 0, 255));

        }
    } else if (mode == 1) {
        findp = false;
        drawp = false;
        cannyVertical(gray, vedge, 20, 100);
        //vedge -= ttt;
        //imshow("vedgdde", vedge);

        //노이즈 제거
        findContours(vedge, ct, RETR_EXTERNAL, CHAIN_APPROX_NONE);
        for (int i = 0; i < ct.size(); i++) {
            double r;
            Point2d c;
            if (ct[i].size() < 0) {
                drawContours(vedge, ct, i, Scalar::all(0), -1);
            } else if (boundingRect(ct[i]).height > 15 &&
                       boundingRect(ct[i]).height < vedge.rows / 2) {
                if (drawCirlceWith3P(vedge, ct[i][ct[i].size() / 64], ct[i][ct[i].size() / 2],
                                     ct[i][ct[i].size() / 4], c, r)) {
                    select.push_back(ct[i]);
                    circles.push_back(Vec3d(c.x, c.y, r));
                } else if (drawCirlceWith3P(vedge, ct[i][ct[i].size() / 32],
                                            ct[i][ct[i].size() / 2], ct[i][ct[i].size() / 8], c,
                                            r)) {
                    select.push_back(ct[i]);
                    circles.push_back(Vec3d(c.x, c.y, r));
                }
            } else {
                drawContours(vedge, ct, i, Scalar::all(0), -1);
            }
        }

        //imshow("vedge", vedge);
        for (int i = 0; i < circles.size() - 1 && circles.size() > 0; i++) {
            for (int j = i + 1; j < circles.size(); j++) {
                if (inCircle(circles[i], circles[j])) {
                    std::vector<Point> temp;
                    temp.insert(temp.begin(), select[i].begin(), select[i].end());
                    temp.insert(temp.end(), select[j].begin(), select[j].end());
                    candies.push_back(temp);
                }
            }
        }

        //후보 영역 그리기
        for (int i = 0; i < candies.size(); i++) {
            Rect rt;
            rt = boundingRect(candies[i]);
            rectangle(vedge, rt, Scalar::all(100));
        }
        //imshow("candies", vedge);

        if (candies.size() > 0) {
            double num = 0;
            int fixels = vedge.cols * vedge.rows;

            Mat mask1, mask2, mask;

            for (int i = 0; i < candies.size(); i++) {
                Point2f c;
                float r;
                minEnclosingCircle(candies[i], c, r);
                Rect rect = boundingRect(candies[i]);
                if (rect.width > rect.height) {

                    if (rr == 0) {
                        if (c.x + r <= vedge.cols * 0.75f && c.x - r >= vedge.cols * 0.25f &&
                            c.y + r <= vedge.rows && c.y - r >= 0 && r < (vedge.cols / 2.0f)) {
                            findp = true;
                            if (candies.size() > 1) {
                                mask1 = Mat::zeros(vedge.size(), vedge.type());
                                mask2 = Mat::zeros(vedge.size(), vedge.type());
                                circle(mask1, c, r, Scalar(255, 255, 255), 10);
                                drawContours(mask2, Mat(candies[i]), -1, Scalar::all(255), -1);
                                bitwise_and(mask1, mask2, mask);
                                double minVal, maxVal;
                                Mat hist = getHistogram(mask, &minVal, &maxVal);
                                double cir_ratio = (fixels - maxVal) / (2.0 * r * CV_PI);
                                if (num < cir_ratio) {
                                    num = cir_ratio;
                                    rr = r;
                                    cc = c;
                                }
                            } else if (candies.size() == 1) {
                                rr = r;
                                cc = c;
                            } else {
                                findp = false;
                            }
                        }
                    } else if (abs(rr - r) <= 3) {
                        //__android_log_print(ANDROID_LOG_DEBUG,"INNN","in");

                        if (c.x + r <= vedge.cols * 0.75f && c.x - r >= vedge.cols * 0.25f &&
                            c.y + r < vedge.rows && c.y - r >= 0 && r < vedge.rows / 2.0f) {
                            findp = true;
                            if (candies.size() > 1) {
                                mask1 = Mat::zeros(vedge.size(), vedge.type());
                                mask2 = Mat::zeros(vedge.size(), vedge.type());
                                circle(mask1, c, r, Scalar(255, 255, 255), 10);
                                drawContours(mask2, Mat(candies[i]), -1, Scalar::all(255), -1);
                                bitwise_and(mask1, mask2, mask);
                                double minVal, maxVal;
                                Mat hist = getHistogram(mask, &minVal, &maxVal);
                                double cir_ratio = (fixels - maxVal) / (2.0 * r * CV_PI);
                                if (num < cir_ratio) {
                                    num = cir_ratio;
                                    rr = r;
                                    cc = c;
                                }
                            } else if (candies.size() == 1) {
                                rr = r;
                                cc = c;
                            } else {
                                findp = false;
                            }
                        }
                    }
                }
            }
            if (findp) {
                circle(dst, cc, rr, Scalar(255, 0, 255));
                //cout << rr << endl;
                drawp = true;
            } else {
                circle(dst, cc, rr, Scalar(0, 0, 255));
                drawp = false;
            }
        }
    }
    // dst=aaa;
    /*else if(mode==2){
        findp = false;
        drawp = false;
        Mat gradient;
        morphologyEx(gray, gradient, MORPH_GRADIENT, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        dst=gradient;
    }*/
}

void cvtContrast(Mat src, Mat &dst, int brightness, int contrast) {
    int var_brightness = brightness - 100;
    int var_contrast = contrast - 100;
    double a, b;
    if (var_contrast > 0) {
        double delta = 127.0 * var_contrast / 100;
        a = 255.0 / (255.0 - delta * 2);
        b = a * (var_brightness - delta);
    } else {
        double delta = -128.0 * var_contrast / 100;
        a = (256.0 - delta * 2) / 255.0;
        b = a * var_brightness + delta;
    }

    src.convertTo(dst, CV_8U, a, b);
}

std::vector<cv::Point2f> getPointPositions(cv::Mat binaryImage) {
    std::vector<cv::Point2f> pointPositions;
    for (unsigned int y = 0; y < binaryImage.rows; ++y) {
        //unsigned char* rowPtr = binaryImage.ptr<unsigned char>(y);
        for (unsigned int x = 0; x < binaryImage.cols; ++x) {
            //if(rowPtr[x] > 0) pointPositions.push_back(cv::Point2i(x,y));
            if (binaryImage.at<unsigned char>(y, x) > 0)
                pointPositions.push_back(cv::Point2f(x, y));
        }
    }
    return pointPositions;
}

void sharpImage(Mat src, Mat &dst, int size) {
    Mat medianb, gau;
    medianBlur(src, medianb, size);
    GaussianBlur(src, gau, Size(0, 0), size, size);
    addWeighted(src, 1.5, medianb, -0.5, 0, medianb);
    addWeighted(medianb, 1.5, gau, -0.5, 0, dst);
}

bool drawCircleWith3P(int width, int height, Point2i p1, Point2i p2, Point2i p3, Point2d &center,
                      double &radius) {     //입력된 3점으로 원을 그림. 그릴 수 있으면 ture, 원의 중심, 원의 반지름 return. 아니면 false return.
    // 점의 좌표를 입력
    double x1 = p1.x;
    double y1 = p1.y;
    double x2 = p2.x;
    double y2 = p2.y;
    double x3 = p3.x;
    double y3 = p3.y;

    // 두 수직 이등분선의 기울기를 입력
    double d1 = (x2 - x1) / (y2 - y1);
    double d2 = (x3 - x2) / (y3 - y2);

    if (isnan(d1))d1 = 0;
    if (isnan(d2))d2 = 0;

    // 원의 중점을 구함
    double cx = ((y3 - y1) + (x2 + x3) * d2 - (x1 + x2) * d1) / (2 * (d2 - d1));
    double cy = -d1 * (cx - (x1 + x2) / 2) + (y1 + y2) / 2;

    if (!(isinf(cx) || isinf(cy) || isnan(cx) || isnan(cy))) {
        double r = sqrt(pow(x1 - cx, 2) + pow(y1 - cy, 2));
        if (r > 14 && r < height * 0.6 // 홍채 사이즈 부분
            && cx + r < width && cx - r >= 0 && cy + r < height && cy - r >= 0) { // 원 범위 부분
            center.x = cx;
            center.y = cy;
            radius = r;
            return true;
        }
    }
    return false;
}

Mat getPointPositions2(Mat binaryImage) {
    Mat pointPositions;
    findNonZero(binaryImage, pointPositions);
    return pointPositions;
}

