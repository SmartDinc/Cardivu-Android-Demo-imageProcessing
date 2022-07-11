#include <jni.h>
#include <android/log.h>
#include <stdio.h>

#include "fft/real_double_fft.h"
#include "fft/stft.h"

using namespace Dsp;

static STFT *stft = NULL;


extern "C"
JNIEXPORT void JNICALL
Java_dsp_STFT_init(JNIEnv *env, jobject instance, jint fftLen, jint samplingRate) {

    if (stft != NULL) delete stft;
    stft = new STFT(fftLen, samplingRate);

}

extern "C"
JNIEXPORT void JNICALL
Java_dsp_STFT_setPreProcessing(JNIEnv *env, jobject instance, jboolean isPreProcessing) {

    if (stft == NULL) {
        char buff[150];
        sprintf(buff, "The STFT is not initialized");
        env->ThrowNew(env->FindClass("java/lang/Exception"), buff);
    }
    stft->set_pre_processing(isPreProcessing);

}

extern "C"
JNIEXPORT void JNICALL
Java_dsp_STFT_feedData(JNIEnv *env, jobject instance, jshortArray ds_, jint dsLen) {

    jshort *elements;
    short *real;
    jboolean is_copy;

    elements = env->GetShortArrayElements(ds_, &is_copy);
    real = (short *) elements;

    stft->feedData(real, dsLen);

    env->ReleaseShortArrayElements(ds_, elements, JNI_FALSE);

}

extern "C"
JNIEXPORT jint JNICALL
Java_dsp_STFT_nElemSpectrumAmp(JNIEnv *env, jobject instance) {

    return stft->get_n_elem_spectrum_amp();

}

extern "C"
JNIEXPORT jdoubleArray JNICALL
Java_dsp_STFT_getSpectrumAmpDB(JNIEnv *env, jobject instance) {

    jdoubleArray resJNI;
    const double *out;
    int out_len;
    jdouble *resArray;
    jboolean is_copy;

    out = stft->getSpectrumAmpDB(out_len);

    resJNI = env->NewDoubleArray(out_len);
    resArray = env->GetDoubleArrayElements(resJNI, &is_copy);

    for (int i = 0; i < out_len; i++) {
        resArray[i] = out[i];
    }

    env->ReleaseDoubleArrayElements(resJNI, resArray, JNI_FALSE);

    return resJNI;

}extern "C"
JNIEXPORT jdoubleArray JNICALL
Java_com_sd_cardivu_1dev_1beta_dsp_DSP_fft(JNIEnv *env, jobject thiz, jdoubleArray ds_,
                                           jint dsLen) {
    // TODO: implement fft()
    // Convert to native format
    jdouble *elements;
    double *real;
    jboolean is_copy_native;

    elements = env->GetDoubleArrayElements(ds_, &is_copy_native);
    real = (double *) elements;

    // FFT
    int ps_len = dsLen / 2;
    double *spectrum_amp_in;
    double *spectrum_amp_out;
    spectrum_amp_in = new double[dsLen];
    spectrum_amp_out = new double[ps_len];
    RealDoubleFFT *spectrum_amp_fft = new RealDoubleFFT(dsLen);
    memcpy(spectrum_amp_in, real, dsLen * sizeof(double));
    spectrum_amp_fft->ft(spectrum_amp_in, dsLen);
    spectrum_amp_fft->fft_to_amp(spectrum_amp_out, spectrum_amp_in, dsLen);

    // Release
    delete spectrum_amp_fft;
    delete spectrum_amp_in;

    // Convert to jni format
    jdoubleArray resJNI;
    jdouble *resArray;
    jboolean is_copy_jni;

    resJNI = env->NewDoubleArray(ps_len);
    resArray = env->GetDoubleArrayElements(resJNI, &is_copy_jni);

    for (int i = 0; i < ps_len; i++) {
        resArray[i] = spectrum_amp_out[i];
    }

    env->ReleaseDoubleArrayElements(resJNI, resArray, JNI_FALSE);

    return resJNI;
}

extern "C"
JNIEXPORT jdouble JNICALL
Java_com_sd_cardivu_1dev_1beta_dsp_DSP_calculate_1df(JNIEnv *env, jobject thiz, jint time) {
    // TODO: implement calculate_df()
    double df = 1 / (double) time;
    return df;
}

extern "C"
JNIEXPORT jdoubleArray JNICALL
Java_com_sd_cardivu_1dev_1beta_dsp_DSP_bandpass(JNIEnv *env, jobject thiz, jdoubleArray ds_,
                                                jint dsLen,
                                                jint fs, jdouble low, jdouble high) {
    // TODO: implement bandpass()
    // Convert to native format
    jdouble *elements;
    double *real;
    jboolean is_copy_native;

    elements = env->GetDoubleArrayElements(ds_, &is_copy_native);
    real = (double *) elements;

    // Calculate parameters for bandpass
    double center = (low + high) / 2;
    double band_width = (high - low);
    int order = 2;

    // Create a 1-channel Butterworth Bandpass of 2,
    // with parameter smoothing and apply it to the audio data.
    Filter *filter = new SmoothedFilterDesign
            <Butterworth::Design::BandPass<2>, 1, DirectFormII>(dsLen);  // <order>로 맞춰줘야함
    Params params;
    params[0] = fs;  // sampling rate
    params[1] = order;  // order
    params[2] = center;  // center frequency
    params[3] = band_width;  // band width
    filter->setParams(params);

    double *data_tmp[1];  // 1 channels
    data_tmp[0] = new double[dsLen];
    memcpy(data_tmp[0], real, dsLen * sizeof(double));  // copy for pre-processing

    filter->process(dsLen, data_tmp);  // bandpass

    // Convert to jni format
    jdoubleArray resJNI;
    jdouble *resArray;
    jboolean is_copy_jni;

    resJNI = env->NewDoubleArray(dsLen);
    resArray = env->GetDoubleArrayElements(resJNI, &is_copy_jni);

    for (int i = 0; i < dsLen; i++) {
        resArray[i] = data_tmp[0][i];
    }

    env->ReleaseDoubleArrayElements(resJNI, resArray, JNI_FALSE);

    return resJNI;
}