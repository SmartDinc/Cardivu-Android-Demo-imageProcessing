package net.sdcor.sdapi.imageprocmodule;

import android.content.Context;
import android.graphics.Bitmap;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;


import net.sdcor.sdapi.server.NowTimeClass;
import net.sdcor.sdapi.server.SD_ServerAlgorithmus;

import org.opencv.android.Utils;
import org.opencv.core.Mat;

import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.atomic.AtomicBoolean;

import static net.sdcor.sdapi.imageprocmodule.cppNetiveImageProcessing.returnPupilSize;

/**
 * image processing
 * server algorithm processing
 * thread 관련된 코드들 여기에
 */
public class ProcessingThread {
    static {
        System.loadLibrary("opencv_java4"); // OpenCV Lib
        System.loadLibrary("native-lib");   // imageProcessing Lib
        System.loadLibrary("dsp-lib");      // imageProcessing Lib
    }

    final String TAG = getClass().getSimpleName();
    Context context;
    boolean isRecord;

    //image processing
    float[] irisCenter;
    static float ref_radius;
    int[] RGB_val;
    boolean endImagePrc;
    ImageProcessThread imagePrc;

    private SD_ServerAlgorithmus mSD_ServerAlgorithmus;
    private HeartDataListener mHeartDataListener;

    //[Sensor]
    SensorManager sm;
    SensorEventListener lightL;
    Sensor lightSensor;
    SensorEventListener accL;
    Sensor accSensor;
    //[Acceleration]
    float diff_accX_Sum = 0;
    float diff_accY_Sum = 0;
    float diff_accZ_Sum = 0;
    float accCnt = 0;
    float curr_accX = 0;
    float curr_accY = 0;
    float curr_accZ = 0;
    float prev_accX = 0;
    float prev_accY = 0;
    float prev_accZ = 0;
    float diff_accXAvg = 0;
    float diff_accYAvg = 0;
    float diff_accZAvg = 0;
    float diff_accXYZAvg = 0;
    double mm_th_mul = 5;
    //[Lux]
    float diff_lux = 0;
    float curr_lux = 127;
    float prev_lux = 0;

    boolean NOISE = false;   //예외처리
    boolean noise_MM = false;   //가속도
    boolean noise_Lx = false;   //조도

    Timer t;//1s 간격 타이머

    public float getCurr_lux() {
        return curr_lux;
    }

    public ProcessingThread(Context context, HeartDataListener mHeartDataListener, SD_ServerAlgorithmus mSD_ServerAlgorithmus, boolean isRecord) {
        this.context = context;
        this.mHeartDataListener = mHeartDataListener;
        this.mSD_ServerAlgorithmus = mSD_ServerAlgorithmus;
        this.isRecord = isRecord;

        NowTimeClass.getNowTime();
    }

    //[start initialize]
    public void initialize() {
        irisCenter = new float[]{-1, -1, 0};//x, y, r
        RGB_val = new int[3];//r, g, b
        ref_radius = 0;

        endImagePrc = false;

        sm = (SensorManager) this.context.getSystemService(Context.SENSOR_SERVICE);
        accSensor = sm.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);     // 가속도
        lightSensor = sm.getDefaultSensor(Sensor.TYPE_LIGHT);     // 조도

        accL = new accListener();       // 가속도 센서 리스너 인스턴스
        lightL = new lightListener();       // 조도 센서 리스너 인스턴스

        sm.registerListener(accL, accSensor, SensorManager.SENSOR_DELAY_NORMAL);        // 가속도 센서 리스너 오브젝트를 등록
        sm.registerListener(lightL, lightSensor, SensorManager.SENSOR_DELAY_FASTEST);    // 조도 센서 리스너 오브젝트를 등록

        imagePrc = new ImageProcessThread();
    }
    //[end initialize]


    public void startProcessing() {
        try {
            FindEyeBuffer.getInstance().clear();
        } catch (Exception e) {
            e.printStackTrace();
        }

        endImagePrc = false;
        imagePrc.start();

        t = new Timer();
        TimerTask task = get1sTimerTask();
        t.schedule(task, 0, 1000);
    }

    public void stopProcessing() {
        endImagePrc = true;

        if (t != null) {
            try {
                t.cancel();
                t = null;
            } catch (Exception e) {
                e.getMessage();
            }
        }

        if (sm != null) {
            try {
                sm.unregisterListener(accL);
                sm.unregisterListener(lightL);
            } catch (Exception e) {
                Log.e(TAG, "stopProcessing: " + e.getMessage());
                e.printStackTrace();
            }
        }

        t.cancel();

        try {
            imagePrc.interrupt();
        } catch (Exception e) {
            e.printStackTrace();
        }

        try {
            FindEyeBuffer.getInstance().init();
        } catch (Exception e) {
            e.printStackTrace();
        }

        imagePrc = null;
    }

    TimerTask get1sTimerTask() {
        return new TimerTask() {
            int gq = 0;
            float bpm = 0;
            float sdnn = 0;
            float rmssd = 0;
            float lf = 0;
            float hf = 0;
            float stress = 0;

            int th_GQdata_Cnt = 0;
            int th_1sec_Cnt = 0;
            int preth_GQdata_Cnt = -1;
            int diff_preth_GQdata_Cnt_th_GQdata_Cnt = 0;

            Date buff_NowDate = null;
            Date buff_PreDate = null;

            @Override
            public void run() {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            buff_NowDate = new Date(NowTimeClass.getNowTime());
                            if (buff_PreDate == null) {
                                buff_PreDate = buff_NowDate;
                            }
//                            Log.e(TAG, "timer_cardivu buff_NowDate.getTime(): " + buff_NowDate.getTime() + "\tbuff_PreDate.getTime() : " + buff_PreDate.getTime());
                            if (buff_NowDate.getTime() - buff_PreDate.getTime() > 500) {
                                buff_PreDate = buff_NowDate;
                                if (mSD_ServerAlgorithmus != null) {
                                    String ResultText = mSD_ServerAlgorithmus.calc_1Hz_resampling();
                                    th_1sec_Cnt++;

                                    if ((preth_GQdata_Cnt == th_GQdata_Cnt)) {
                                        diff_preth_GQdata_Cnt_th_GQdata_Cnt++;
                                        if (mHeartDataListener != null) {
                                            mHeartDataListener.goWait60s(diff_preth_GQdata_Cnt_th_GQdata_Cnt);
                                        }
                                    } else {
                                        diff_preth_GQdata_Cnt_th_GQdata_Cnt = 0;
                                        preth_GQdata_Cnt = th_GQdata_Cnt;
                                    }

                                    if (mHeartDataListener != null) {
                                        mHeartDataListener.onGetData(ResultText); //callback
                                    }

                                    if (mHeartDataListener != null) {
                                        mHeartDataListener.setLux5_low(getCurr_lux());
                                    }
                                    diff_accX_Sum = 0;
                                    diff_accY_Sum = 0;
                                    diff_accZ_Sum = 0;
                                    accCnt = 0;
                                    noise_MM = false;
                                }


                            } else {
                                buff_PreDate = buff_NowDate;
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                            Log.e(TAG, e.getMessage());
                        }
                    }
                }).start();
            }
        };
    }

    //[start image processing]
    public class ImageProcessThread extends Thread {
        FindEyeBuffer buffer;
        AtomicBoolean isPrc;

        public ImageProcessThread() {
            buffer = FindEyeBuffer.getInstance();
            isPrc = new AtomicBoolean(false);
        }

        @Override
        public void run() {
            super.run();

            try {
                while (!endImagePrc) {
                    try {
                        if (isPrc.get()) continue;
                        if (!buffer.isEmpty()) {
                            FindEyeData data;
                            Mat matInput;
                            try {
                                isPrc.set(true);
                                data = buffer.remove();
                                if (data == null) {
                                    try {
                                        Thread.sleep(30);
                                    } catch (Exception e) {
                                        if (e.getMessage() != null) {
                                            Log.e(TAG, "sleep_e - Thread: " + e.getMessage());
                                        }
                                        e.printStackTrace();
                                    }
                                    continue;
                                }

                                matInput = new Mat();
                                Bitmap bitmap = data.getBitmap();
                                if (bitmap == null) {
                                    try {
                                        Thread.sleep(30);
                                    } catch (Exception sleep_e) {
                                        if (sleep_e.getMessage() != null) {
                                            Log.e(TAG, "sleep_e - Thread: " + sleep_e.getMessage());
                                        }
                                    }
                                    continue;
                                }
                                Utils.bitmapToMat(bitmap, matInput);
                            } catch (Exception e) {
                                Log.e(TAG, "Bitmap To Mat " + e.getMessage());
                                e.printStackTrace();
                                try {
                                    Thread.sleep(30);
                                } catch (Exception sleep_e) {
                                    if (e.getMessage() != null) {
                                        Log.e(TAG, "sleep_e - Thread: " + e.getMessage());
                                    }
                                    e.printStackTrace();
                                    sleep_e.printStackTrace();
                                }
                                continue;
                            }
                            float[] facer = new float[]{data.getFaceRect().left, data.getFaceRect().top, data.getFaceRect().right, data.getFaceRect().bottom};
                            float[] leftr = new float[]{data.getlEyeRect().left, data.getlEyeRect().top, data.getlEyeRect().right, data.getlEyeRect().bottom, data.getlEyeCenter().x, data.getlEyeCenter().y};
                            float[] OF_array = new float[244];

                            boolean isPupil = returnPupilSize(matInput.getNativeObjAddr(), null, facer, leftr, leftr, irisCenter, OF_array, ref_radius, RGB_val);

                            if (isPupil) {
                                double[] features = new double[244];
                                for (int i = 0; i < 244; i++) {
                                    features[i] = OF_array[i];
                                }

                                mSD_ServerAlgorithmus.setVarRGB(RGB_val);
                                mSD_ServerAlgorithmus.feed(features, getNoseState());
                            }
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    } finally {
                        try {
                            Thread.sleep(30);
                        } catch (Exception e) {
                            Log.e(TAG, "sleep_e - Thread: " + e.getMessage());
                            e.printStackTrace();
                        }
                        isPrc.set(false);
                        continue;

                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                endImagePrc = true;
            }
        }
    }
    //[end image processing]

    //[start SensorEvent]
    private class lightListener implements SensorEventListener {      //LDW 20191223 조도
        public void onSensorChanged(SensorEvent event) {
            try {
                curr_lux = event.values[0];
                diff_lux = Math.abs(prev_lux - curr_lux);
                prev_lux = curr_lux;

                // 조도 임계값
                if (curr_lux <= 20 || (diff_lux / curr_lux) > 0.33) {
                    noise_Lx = true;
                } else {
                    noise_Lx = false;
                }
//                Log.e(TAG, "getNoseState:\t noise_Lx: " + noise_Lx + "\tcurr_lux: " + curr_lux + "\tdiff_lux: " + diff_lux);

            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        public void onAccuracyChanged(Sensor sensor, int accuracy) {
        }
    }

    private class accListener implements SensorEventListener {
        @Override
        public void onSensorChanged(SensorEvent event) {
            try {
                curr_accX = event.values[0];
                curr_accY = event.values[1];
                curr_accZ = event.values[2];

                diff_accX_Sum += Math.abs(prev_accX - curr_accX);
                diff_accY_Sum += Math.abs(prev_accY - curr_accY);
                diff_accZ_Sum += Math.abs(prev_accZ - curr_accZ);

                prev_accX = curr_accX;
                prev_accY = curr_accY;
                prev_accZ = curr_accZ;

                accCnt++;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        public void onAccuracyChanged(Sensor sensor, int accuracy) {
        }
    }

    private boolean getNoseState() {
        diff_accXAvg = (5 * diff_accX_Sum) / accCnt;   //가속도
        diff_accYAvg = diff_accY_Sum / accCnt;   //가속도
        diff_accZAvg = diff_accZ_Sum / accCnt;   //가속도

        diff_accXYZAvg = (diff_accXAvg + diff_accYAvg + diff_accZAvg) / 3;

        if ((diff_accXAvg > (0.1 * mm_th_mul) && diff_accYAvg > (0.05 * mm_th_mul) && diff_accZAvg > (0.2 * mm_th_mul)) || (diff_accXYZAvg > (0.3 * mm_th_mul))) {
            noise_MM = true;
        }

        NOISE = noise_Lx || noise_MM;
//        Log.e(TAG, "getNoseState: NOISE: " + NOISE + "\t noise_Lx " + noise_Lx + "\tnoise_MM" + noise_MM);
        return NOISE;
    }
    //[end SensorEvent]

}
