package net.sdcor.sdapi.server;

import android.os.AsyncTask;

public class NowTimeClass {
    static long timeServer_time = 0;
    static long system_time = 0;

    public static long getNowTime() {
        long nowTime;
        try {
            if (timeServer_time == 0) {
                try {
                    timeServer_time = new getNowTimeAsyncTask().execute().get();
                    system_time = System.currentTimeMillis();
                } catch (Exception e) {
                    e.printStackTrace();
                    timeServer_time = 0;

                    return 0;
                }
            }

            nowTime = timeServer_time + System.currentTimeMillis() - system_time;

        } catch (Exception e) {
            e.printStackTrace();
            nowTime = 0;
        }

        return nowTime;
    }

    public static class getNowTimeAsyncTask extends AsyncTask<String, Void, Long> {

        @Override
        protected Long doInBackground(String... params) {
            return System.currentTimeMillis();
        }

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
        }

        @Override
        protected void onPostExecute(Long result) {
            super.onPostExecute(result);
        }
    }

}
