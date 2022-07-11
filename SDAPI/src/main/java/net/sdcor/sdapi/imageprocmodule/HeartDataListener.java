package net.sdcor.sdapi.imageprocmodule;

public interface HeartDataListener {
    void onGetData(String ResultTextList);

    void goWait60s(int diffCount);

    void setLux5_low(float Lux);
}