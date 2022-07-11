package net.sdcor.cardivu_android_demo.sdapi;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.util.Size;
import android.view.WindowManager;
import android.widget.ImageView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.camera.camera2.Camera2Config;
import androidx.camera.core.AspectRatio;
import androidx.camera.core.CameraInfo;
import androidx.camera.core.CameraSelector;
import androidx.camera.core.CameraXConfig;
import androidx.camera.core.ImageAnalysis;
import androidx.camera.core.Preview;
import androidx.camera.core.VideoCapture;
import androidx.camera.lifecycle.ProcessCameraProvider;

import androidx.camera.view.PreviewView;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.core.content.ContextCompat;
import androidx.lifecycle.LifecycleOwner;


import com.google.android.gms.vision.CameraSource;
import com.google.common.util.concurrent.ListenableFuture;


import net.sdcor.sdapi.imageprocmodule.GraphicOverlay;

import java.io.File;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class CameraManager extends AppCompatActivity implements CameraXConfig.Provider {
    final String TAG = this.getClass().getSimpleName();
    final int Width = 480;
    final int Height = 640;
    Context context;
    PreviewView previewView;
    Preview preview;
    ImageView mImageView;

    private ExecutorService cameraExecutor;
    CameraAnalyzerManager analyzer;

    ConstraintLayout measure_top;
    WindowManager.LayoutParams windowParams;
    WindowManager mWindowManager;
    private GraphicOverlay graphicOverlay;

    Boolean isVideoCapture = false;
    onBitmapListener mOnBitmapListener;
    CameraInfo mCameraInfo;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    // View
    public CameraManager(Context context, ConstraintLayout measure_top, Boolean isVideoCapture) {
        this.context = context;
        this.measure_top = measure_top;
        this.mWindowManager = null;
        this.isVideoCapture = isVideoCapture;

        setupView(isVideoCapture);
        init();
    }

    private void init() {
        if (this.isVideoCapture) {
            mOnBitmapListener = new onBitmapListener() {

                @Override
                public void setBitmap(Bitmap bitmap) {
                    try {
                        Matrix sideInversion = new Matrix();
                        sideInversion.setScale(-1, 1); // 좌우반전

                        Bitmap sideInversionImg = Bitmap.createBitmap(bitmap, 0, 0,
                                bitmap.getWidth(), bitmap.getHeight(), sideInversion, false);
                        if (mImageView != null && sideInversionImg != null) {
                            mImageView.setImageBitmap(sideInversionImg);
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }

                }
            };
            analyzer = new CameraAnalyzerManager(graphicOverlay, mOnBitmapListener);
        } else {
            mOnBitmapListener = null;
            analyzer = new CameraAnalyzerManager(graphicOverlay);
        }
    }

    private void setupView(Boolean isVideoCapture) {
        try {
            windowParams = new WindowManager.LayoutParams(Width, Height);
            previewView = new PreviewView(this.context);
            measure_top.addView(previewView, windowParams);
            if (isVideoCapture) {    // 영상녹화
                Log.e(TAG, "isVideoCapture: " + isVideoCapture);
                mImageView = new ImageView(context);
                measure_top.addView(mImageView, windowParams);
            }
            graphicOverlay = new GraphicOverlay(context, null);
            graphicOverlay.setCameraInfo(Width, Height, CameraSource.CAMERA_FACING_FRONT);
            measure_top.addView(graphicOverlay, windowParams);
        } catch (Exception e) {
            Log.e(TAG, "setupView: " + e.getMessage());
            e.printStackTrace();
        }
    }

    @NonNull
    @Override
    public CameraXConfig getCameraXConfig() {
        return Camera2Config.defaultConfig();
    }

    private ListenableFuture<ProcessCameraProvider> cameraProviderFuture;

    public void setUpCamera() {
        cameraProviderFuture = ProcessCameraProvider.getInstance(context);
        cameraProviderFuture.addListener(() -> {
            try {
                ProcessCameraProvider cameraProvider = cameraProviderFuture.get();
                bindPreview(cameraProvider);
            } catch (ExecutionException | InterruptedException e) {
                e.printStackTrace();
            }
        }, ContextCompat.getMainExecutor(context));
    }

    @SuppressLint("RestrictedApi")
    void bindPreview(@NonNull ProcessCameraProvider cameraProvider) {
        cameraExecutor = Executors.newSingleThreadExecutor();
        CameraSelector cameraSelector = new CameraSelector.Builder()
                .requireLensFacing(CameraSelector.LENS_FACING_FRONT)
                .build();

        HandlerThread analyzerThread = new HandlerThread("FaceDetectionAnalyzer");
        analyzerThread.start();

        ImageAnalysis imageAnalysis =
                new ImageAnalysis.Builder()
                        .setTargetResolution(new Size(Width, Height))
                        .build();

        imageAnalysis.setAnalyzer(cameraExecutor, analyzer);

        try {
            cameraProvider = cameraProviderFuture.get();
        } catch (ExecutionException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            cameraProvider.unbindAll();
        }

        if (isVideoCapture) {    // 영상녹화
            VideoCapture videoCapture = new VideoCapture.Builder()
                    .setTargetRotation(previewView.getDisplay().getRotation())
                    .build();
            cameraProvider.bindToLifecycle(((LifecycleOwner) ((Context) context)), cameraSelector, imageAnalysis, videoCapture);
            startVideoCapture(videoCapture);
        } else {
            preview = new Preview.Builder()
                    .setTargetAspectRatio(AspectRatio.RATIO_4_3)
                    .build();
            preview.setSurfaceProvider(previewView.getSurfaceProvider());
            cameraProvider.bindToLifecycle(((LifecycleOwner) ((Context) context)), cameraSelector, preview, imageAnalysis);
        }
        setmCameraInfo(imageAnalysis.getCamera().getCameraInfo());
    }

    @SuppressLint({"MissingPermission", "RestrictedApi"})
    public void startVideoCapture(VideoCapture videoCapture) {
        try {
            File[] externalStorageVolumes = ContextCompat.getExternalFilesDirs(context.getApplicationContext(), null);
            Log.e(TAG, "externalStorageVolumes: " + externalStorageVolumes.length);
            Log.e(TAG, "externalStorageVolumes: " + externalStorageVolumes[0] + "\t" + externalStorageVolumes[1] + "\t");

            String FolderPath = externalStorageVolumes[1] + "/CardiVu_video/";
            Log.e(TAG, "FolderPath: " + FolderPath);
            File saveFile = new File(FolderPath); // 저장 경로
            if (!saveFile.exists()) {
                saveFile.mkdir();
            }
            String videoFilePath = FolderPath + LocalDateTime.now().format(DateTimeFormatter.ISO_DATE_TIME) + ".mp4"; // 저장 경로

            VideoCapture.OutputFileOptions outputFileOptions =
                    new VideoCapture.OutputFileOptions.Builder(new File(videoFilePath)).build();

            videoCapture.startRecording(
                    outputFileOptions,
                    Executors.newSingleThreadExecutor(),
                    new VideoCapture.OnVideoSavedCallback() {
                        @Override
                        public void onVideoSaved(@NonNull VideoCapture.OutputFileResults outputFileResults) {
                            Log.e(TAG, "onVideoSaved: " + outputFileResults.toString());
                        }

                        @Override
                        public void onError(int videoCaptureError, @NonNull String message, @Nullable Throwable cause) {
                            Log.e(TAG, "onError: " + message);
                            if (cause != null) {
                                cause.printStackTrace();
                                videoCapture.stopRecording();
                            }
                        }
                    });
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @SuppressLint("RestrictedApi")
    public void stop() {
        Log.e(TAG, "stop");
        try {
            Handler handler = new Handler(Looper.getMainLooper()) {
                @Override
                public void handleMessage(Message msg) {
                    if (previewView != null) {
                        try {
                            previewView.clearAnimation();
                            preview.onDetached();
                        } catch (Exception e) {
                            Log.e(TAG, "stop - preview: " + e.getMessage());
                            e.printStackTrace();
                        }
                    }
                    if (graphicOverlay != null) {
                        graphicOverlay.clear();
                    }

                    if (mWindowManager != null) {
                        if (graphicOverlay != null) {
                            try {
                                mWindowManager.removeView(graphicOverlay);
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                        }
                        if (previewView != null) {
                            try {
                                mWindowManager.removeView(previewView);
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }
            };
            handler.sendEmptyMessage(0);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void setmCameraInfo(CameraInfo mCameraInfo) {
        this.mCameraInfo = mCameraInfo;
    }
}
