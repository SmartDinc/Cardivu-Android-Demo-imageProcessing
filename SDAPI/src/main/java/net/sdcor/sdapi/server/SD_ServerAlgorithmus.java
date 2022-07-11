package net.sdcor.sdapi.server;

import android.util.Log;

import java.text.SimpleDateFormat;
import java.util.Arrays;

public class SD_ServerAlgorithmus {
    final String TAG = this.getClass().getSimpleName();

    private int frame_count = 0;
    private int __bpm_window_size__ = 30;
    private int __feature_length__ = 244;
    private boolean noise = false;

    double[][] OF_buffer;
    double[] OF_sum;
    double[] OF_1Hz_array;
    double[] Save_OF_1Hz_array;
    int[] varRGB;

    public void setVarRGB(int[] varRGB) {
        this.varRGB = varRGB;
    }

    public SD_ServerAlgorithmus() {
        OF_buffer = new double[30][244];
        OF_sum = new double[244];
        OF_1Hz_array = new double[244];
        Save_OF_1Hz_array = new double[244];
        varRGB = new int[3];
    }

    // -------------------------------------------------------------------------Signal Processing
    // frame 마다
    public void feed(double[] optical_flow_features, boolean noise) {
        try {
            this.noise = noise;
//            Log.e("feed", "noise: " + noise + "\tframe_count:" + frame_count);
            if (!noise) {
                if (!(OF_buffer == null || OF_sum == null || OF_1Hz_array == null || optical_flow_features == null)) {
                    calc_sum(optical_flow_features);
                    frame_count++;
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "feed: " + e.getMessage());
        }
    }

    private void calc_sum(double[] array) {
        if (frame_count >= __bpm_window_size__) return;

        for (int j = 0; j < __feature_length__; j++) {
            OF_buffer[frame_count][j] += array[j];
            OF_sum[j] += array[j];
        }
    }

    private void calc_SD_noise_rejection() {
        double[] OF_Avg = new double[__feature_length__];
        double[] OF_SD = new double[__feature_length__];

        for (int j = 0; j < __feature_length__; j++) {
            double[] temparray = new double[frame_count_1Hz];
            if (frame_count_1Hz < 30) {
                for (int i = 0; i < frame_count_1Hz; i++) {
                    temparray[i] = OF_buffer[i][j];
                }
            } else {
                frame_count_1Hz = 29;
                for (int i = 0; i < frame_count_1Hz; i++) {
                    temparray[i] = OF_buffer[i][j];
                }
            }

            OF_Avg[j] = OF_sum[j] / frame_count_1Hz;
            OF_SD[j] = analysis_sdnn(temparray, frame_count_1Hz, 1);


            for (int i = 0; i < frame_count_1Hz; i++) {
                if ((OF_buffer[i][j] > (OF_Avg[j] + OF_SD[j] * 2.5)) ||
                        (OF_buffer[i][j] < (OF_Avg[j] - OF_SD[j] * 2.5))) {
                    OF_buffer[i][j] = -99999;
                }
            }
        }
    }

    int frame_count_1Hz = 0;

    public String calc_1Hz_resampling() {
        String ResultText = "";

        frame_count_1Hz = frame_count;

//        Log.e(TAG, "calc_1Hz_resampling:\t!noise: " + noise + "\tframe_count: " + frame_count);
        if ((!noise) && frame_count_1Hz > 3) {

            calc_SD_noise_rejection();

            double[] OF_buffer_Sum = new double[__feature_length__];
            double OF_buffer_Cnt = 0;

            boolean OF_Noies = false;

            for (int i = 0; i < frame_count_1Hz; i++) {
                for (int j = 0; j < __feature_length__; j++) {
                    if (OF_buffer[i][j] == -99999) {
                        OF_Noies = true;
                        break;
                    }
                    OF_buffer_Sum[j] += OF_buffer[i][j];
                }

                if (!OF_Noies) {
                    OF_buffer_Cnt++;
                } else {
                    for (int j = 0; j < __feature_length__; j++) {
                        if (OF_buffer[i][j] == -99999) {
                            break;
                        }
                        OF_buffer_Sum[j] -= OF_buffer[i][j];
                    }
                }
                OF_Noies = false;
            }

            //1초당 PupilSize
            if (OF_buffer_Cnt >= 3) {
                for (int j = 0; j < __feature_length__; j++) {
                    OF_1Hz_array[j] = getMathAround((OF_buffer_Sum[j] / OF_buffer_Cnt), 6);   //20200707
                    Save_OF_1Hz_array[j] = OF_1Hz_array[j];
                }
            } else {
                for (int j = 0; j < __feature_length__; j++) {
                    OF_1Hz_array[j] = 0;
                    Save_OF_1Hz_array[j] = OF_1Hz_array[j];
                }
            }
            for (int j = 0; j < __feature_length__; j++) {
                OF_1Hz_array[j] = 0;
            }

            if (!(Save_OF_1Hz_array[0] == 0 && Save_OF_1Hz_array[1] == 0 && Save_OF_1Hz_array[2] == 0)) {
                ResultText = Arrays.toString(Save_OF_1Hz_array);
            }

        }

        frame_count_1Hz = 0;
        frame_count = 0;

        OF_buffer = new double[30][244];
        OF_sum = new double[244];
        OF_1Hz_array = new double[244];

        return ResultText;
    }

    double analysis_mean(double[] array, int size) {
        double sum = 0.0;

        for (int i = 0; i < size; i++)
            sum += array[i];

        return sum / size;
    }

    double analysis_sdnn(double[] array, int size, int option) {
        if (size < 2) return Math.sqrt(-1.0);

        double sum = 0.0;
        double sd;
        double meanValue = analysis_mean(array, size);

        for (int i = 0; i < size; i++) {
            sum += Math.pow((array[i] - meanValue), 2);
        }
        sd = Math.sqrt(sum / (size - option));

        return sd;
    }

    double getMathAround(double targetValue, int position) {
        double ndb = Math.pow(10.0, position);

        return (Math.round(targetValue * ndb) / ndb);
    }
}
