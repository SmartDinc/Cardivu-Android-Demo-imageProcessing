package net.sdcor.sdapi.imageprocmodule;


public class cppNetiveImageProcessing {
    static {
        System.loadLibrary("opencv_java4"); // OpenCV Lib
        System.loadLibrary("native-lib");   // imageProcessing Lib
        System.loadLibrary("dsp-lib");      // imageProcessing Lib
    }

    public static native boolean returnPupilSize(long matAddrInput, int[] illu, float[] face, float[] leftEye, float[] rightEye, float[] irisCenter, float[] OF_arry, float ref_radius, int[] RGB_val);
}