package net.sdcor.cardivu_android_demo.sdapi;

import android.annotation.SuppressLint;
import android.media.Image;
import android.util.Log;

import androidx.camera.core.ImageAnalysis;
import androidx.camera.core.ImageProxy;

import com.google.firebase.ml.vision.common.FirebaseVisionImage;
import com.google.firebase.ml.vision.common.FirebaseVisionImageMetadata;

import net.sdcor.sdapi.imageprocmodule.GraphicOverlay;
import net.sdcor.sdapi.imageprocmodule.ModuleLinkedCamera;


public class CameraAnalyzerManager implements ImageAnalysis.Analyzer {
    String TAG = getClass().getSimpleName();

    GraphicOverlay graphicOverlay;
    ModuleLinkedCamera moduleLinkedCamera;
    onBitmapListener mOnBitmapListener;

    public CameraAnalyzerManager(GraphicOverlay graphicOverlay, onBitmapListener mOnBitmapListener) {
        this.graphicOverlay = graphicOverlay;
        this.mOnBitmapListener = mOnBitmapListener;

        moduleLinkedCamera = new ModuleLinkedCamera();
        moduleLinkedCamera.initialFunction(this.graphicOverlay);
        moduleLinkedCamera.setFrameNum(7);
    }

    public CameraAnalyzerManager(GraphicOverlay graphicOverlay) {
        this.graphicOverlay = graphicOverlay;
        this.mOnBitmapListener = null;

        moduleLinkedCamera = new ModuleLinkedCamera();
        moduleLinkedCamera.initialFunction(this.graphicOverlay);
        moduleLinkedCamera.setFrameNum(7);
    }

    @Override
    public void analyze(ImageProxy imageProxy) {
        try {
            if (imageProxy == null) return;
            int degrees = imageProxy.getImageInfo().getRotationDegrees();

            @SuppressLint({"UnsafeExperimentalUsageError", "UnsafeOptInUsageError"})
            Image mediaImage = imageProxy.getImage();

            int rotation = degreesToFirebaseRotation(degrees);
            FirebaseVisionImage image = FirebaseVisionImage.fromMediaImage(mediaImage, rotation);
            imageProxy.close();

            if(image != null){
                moduleLinkedCamera.inputImage(image);
                if (mOnBitmapListener != null) {
                    mOnBitmapListener.setBitmap(image.getBitmap());
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "imageProcessing: " + e.getMessage());
            e.printStackTrace();
            if (imageProxy != null)
                imageProxy.close();
        }
    }

    private int degreesToFirebaseRotation(int degrees) {
        switch (degrees) {
            case 0:
                return FirebaseVisionImageMetadata.ROTATION_0;
            case 90:
                return FirebaseVisionImageMetadata.ROTATION_90;
            case 180:
                return FirebaseVisionImageMetadata.ROTATION_180;
            case 270:
                return FirebaseVisionImageMetadata.ROTATION_270;
            default:
                throw new IllegalArgumentException(
                        "Rotation must be 0, 90, 180, or 270.");
        }
    }
}