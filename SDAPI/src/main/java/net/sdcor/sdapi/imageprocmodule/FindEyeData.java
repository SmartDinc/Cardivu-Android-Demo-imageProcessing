package net.sdcor.sdapi.imageprocmodule;

import android.graphics.Bitmap;
import android.graphics.PointF;
import android.graphics.RectF;

import com.google.firebase.ml.vision.face.FirebaseVisionFace;

public class FindEyeData {
    private FirebaseVisionFace face;
    private RectF faceRect;
    private RectF lEyeRect;
    private RectF rEyeRect;
    private PointF lEyeCenter;
    private PointF rEyeCenter;
    private Bitmap bitmap;
    private boolean lastFrame; //RY

    FindEyeData(FirebaseVisionFace face, RectF faceRect, RectF lEyeRect, RectF rEyeRect, PointF lEyeCenter, PointF rEyeCenter, Bitmap bitmap, boolean lastFrame) {
        this.face = face;
        this.faceRect = faceRect;
        this.lEyeRect = lEyeRect;
        this.rEyeRect = rEyeRect;
        this.lEyeCenter = lEyeCenter;
        this.rEyeCenter = rEyeCenter;
        this.bitmap = bitmap;
        this.lastFrame = lastFrame;
    }

    public RectF getFaceRect() {
        return faceRect;
    }

    public RectF getlEyeRect() {
        return lEyeRect;
    }

    public PointF getlEyeCenter() {
        return lEyeCenter;
    }

    public Bitmap getBitmap() {
        return bitmap;
    }

    public boolean isLastFrame() {
        return lastFrame;
    }

    @Override
    public String toString() {
        return "isLastFrame=" + lastFrame + ", FindEyeData{ face = " + face +
                ", faceRect=" + faceRect +
                ", lEyeRect=" + lEyeRect +
                ", rEyeRect=" + rEyeRect +
                ", lEyeCenter=" + lEyeCenter +
                ", rEyeCenter=" + rEyeCenter +
                ", bitmap=" + bitmap + "}";
    }
}
