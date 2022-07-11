// Copyright 2018 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package net.sdcor.sdapi.imageprocmodule;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.PointF;
import android.graphics.RectF;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.View;

import com.google.firebase.ml.vision.common.FirebaseVisionPoint;
import com.google.firebase.ml.vision.face.FirebaseVisionFace;
import com.google.firebase.ml.vision.face.FirebaseVisionFaceLandmark;


/**
 * Graphic instance for rendering face position, orientation, and landmarks within an associated
 * graphic overlay view.
 */
public class FaceGraphic extends GraphicOverlay.Graphic {
    final String TAG = this.getClass().getSimpleName();

    private static final float ID_TEXT_SIZE = 45.0f;
    private static final float BOX_STROKE_WIDTH = 5.0f;

    static int numClose = 0;
    static int faceCount = 0;
    static int eyesCount = 0;
    static boolean isEyeTraking = false;
    static boolean isFocus = true;

    private final Paint facePositionPaint;
    private final Paint idPaint;
    private final Paint boxPaint;
    private final Paint boxPaint_;
    private Paint idPaint2;
    private Paint luxPaint;

    private volatile FirebaseVisionFace face;
    private Bitmap bitmap;

    private RectF faceRect;
    private RectF leftEye;
    private RectF RightEye;

    final String closeEyeAlertMessage = "Please open your eyes wide";
    static float lux = -1f;
    static int gq = -1;

    boolean enableDraw = true;
    boolean showAlert = true;

    private FindEyeBuffer findEyeBuffer;

    private boolean isSave = false;
    boolean isLast = false;

    public int selectedColor = Color.WHITE;

    public static void setGq(int buffgq) {
        gq = buffgq;
    }

    public FirebaseVisionFace getFace() {
        return face;
    }

    public static int getNumClose() {
        return numClose;
    }

    public static int getFaceCount() {
        return faceCount;
    }

    public static int getEyesCount() {
        return eyesCount;
    }

    public static boolean isIsEyeTraking() {
        return isEyeTraking;
    }

    public static void setIsEyeTraking(boolean isEyeTraking) {
        FaceGraphic.isEyeTraking = isEyeTraking;
    }

    public static boolean isIsFocus() {
        return isFocus;
    }

    public static void setIsFocus(boolean isFocus) {
        FaceGraphic.isFocus = isFocus;
    }

    public FaceGraphic(GraphicOverlay overlay) {
        super(overlay);
        numClose = 0;
        faceCount = 0;
        eyesCount = 0;

        findEyeBuffer = FindEyeBuffer.getInstance();

        facePositionPaint = new Paint();
        facePositionPaint.setColor(selectedColor);

        idPaint = new Paint();
        idPaint.setColor(selectedColor);
        idPaint.setTextSize(ID_TEXT_SIZE);
        idPaint2 = new Paint();
        idPaint2.setTextSize(ID_TEXT_SIZE);
        idPaint2.setColor(Color.BLACK);

        boxPaint = new Paint();
        boxPaint.setColor(selectedColor);
        boxPaint.setStyle(Paint.Style.STROKE);
        boxPaint.setStrokeWidth(BOX_STROKE_WIDTH);

        boxPaint_ = new Paint();
        boxPaint_.setColor(Color.GREEN);
        boxPaint_.setStyle(Paint.Style.STROKE);
        boxPaint_.setStrokeWidth(BOX_STROKE_WIDTH);

        luxPaint = new Paint();
        luxPaint.setColor(Color.RED);
        luxPaint.setTextSize(40f);

        gq = -1;
    }

    void setData(FirebaseVisionFace face, Bitmap bitmap, boolean isLast) {
        this.face = null;
        this.bitmap = null;
        this.face = face;
        this.bitmap = bitmap;
        this.isSave = true;
        this.isLast = isLast;
    }

    /**
     * Draws the face annotations for position on the supplied canvas.
     */
    @Override
    public void draw(Canvas canvas) {
        try {
            this.setDrawFace(canvas);

            if (lux >= 0f) {
                canvas.drawText("" + lux, 10, 60, luxPaint);
                lux = -1f;
            }
            if (gq > 0) {
                int tempGQ = Math.round((gq / 180.f) * 100.f);
                if (tempGQ > 0) canvas.drawText((tempGQ + "%"), 10, 120, luxPaint);
            }

            if (enableDraw && (faceRect != null)) {
                faceCount++;
                canvas.drawRect(faceRect, boxPaint);
            }

            if (enableDraw && (leftEye != null) && (RightEye != null)) {
                if (isIsEyeTraking()) {
                    if (isIsFocus()) {
                        eyesCount++;
                    }
//                    canvas.drawRect(leftEye, boxPaint_);
//                    canvas.drawRect(RightEye, boxPaint_);
                }

                // 좌우 얼굴 각도
//            Log.e(TAG, "face: " + face.getLeftEyeOpenProbability() + "\t" + face.getHeadEulerAngleY() + "\t" + face.getHeadEulerAngleZ());

            }
//                Log.e(TAG, "focus: " + FaceGraphic.getFaceCount() + "\t" + FaceGraphic.getFaceCount());
        } catch (Exception e) {
            e.printStackTrace();
            Log.e(TAG, e.getMessage());
        }
    }

    private void setDrawFace(Canvas canvas) {
//        Handler handler = new Handler(Looper.getMainLooper()) {
//            @Override
//            public void handleMessage(Message msg) {
        try {
            FirebaseVisionFace face = getFace();

            float x = translateX(face.getBoundingBox().centerX());
            float y = translateY(face.getBoundingBox().centerY());

            float xOffset = scaleX(face.getBoundingBox().width() / 2.0f);
            float yOffset = scaleY(face.getBoundingBox().height() / 2.0f);
            float left = x - xOffset;
            float top = y - yOffset;
            float right = x + xOffset;
            float bottom = y + yOffset;

            faceRect = new RectF(left, top, right, bottom);

            if (face != null) {
                FirebaseVisionFaceLandmark left_land = face.getLandmark(FirebaseVisionFaceLandmark.LEFT_EYE);
                FirebaseVisionFaceLandmark right_land = face.getLandmark(FirebaseVisionFaceLandmark.RIGHT_EYE);
                FirebaseVisionFaceLandmark nose_land = face.getLandmark(FirebaseVisionFaceLandmark.NOSE_BASE);

                if (!(left_land == null || right_land == null || nose_land == null)) {
                    FirebaseVisionPoint left_p = left_land.getPosition();
                    FirebaseVisionPoint right_p = right_land.getPosition();
                    FirebaseVisionPoint center_p = new FirebaseVisionPoint((left_p.getX() + right_p.getX()) * 0.5f, (left_p.getY() + right_p.getY()) * 0.5f, 0f);
                    FirebaseVisionPoint nose_p = nose_land.getPosition();

                    FirebaseVisionPoint front_leye_point = new FirebaseVisionPoint(left_p.getX() + (center_p.getX() - left_p.getX()) * 0.6f, (center_p.getY() + left_p.getY()) * 0.5f, 0f);
                    FirebaseVisionPoint end_leye_point = new FirebaseVisionPoint(left_p.getX() * 2f - front_leye_point.getX(), getRotatedY(left_p, right_p, left_p.getX() * 2f - front_leye_point.getX(), 0), 0f);
                    FirebaseVisionPoint front_reye_point = new FirebaseVisionPoint(right_p.getX() - (right_p.getX() - center_p.getX()) * 0.6f, (right_p.getY() + center_p.getY()) * 0.5f, 0f);
                    FirebaseVisionPoint end_reye_point = new FirebaseVisionPoint(right_p.getX() * 2f - front_reye_point.getX(), getRotatedY(left_p, right_p, right_p.getX() * 2f - front_reye_point.getX(), 0), 0f);
                    FirebaseVisionPoint bottom_leye_point = new FirebaseVisionPoint(left_p.getX(), left_p.getY() + (front_leye_point.getX() - left_p.getX()), 0f);
                    FirebaseVisionPoint bottom_reye_point = new FirebaseVisionPoint(right_p.getX(), right_p.getY() + (right_p.getX() - front_reye_point.getX()), 0f);
                    FirebaseVisionPoint top_leye_point = new FirebaseVisionPoint(left_p.getX(), left_p.getY() - (front_leye_point.getX() - left_p.getX()), 0f);
                    FirebaseVisionPoint top_reye_point = new FirebaseVisionPoint(right_p.getX(), right_p.getY() - (right_p.getX() - front_reye_point.getX()), 0f);

                    leftEye = new RectF(translateX(front_leye_point.getX()), translateY(top_leye_point.getY()), translateX(end_leye_point.getX()), translateY(bottom_leye_point.getY()));
                    RightEye = new RectF(translateX(front_reye_point.getX()), translateY(top_reye_point.getY()), translateX(end_reye_point.getX()), translateY(bottom_reye_point.getY()));

                    // Angle
                    float nose_x = translateX(nose_p.getX());
                    float nose_y = translateY(nose_p.getY());

                    float right_x = translateX(right_p.getX());
                    float right_y = translateY(right_p.getY());

                    float left_x = translateX(left_p.getX());
                    float left_y = translateY(left_p.getY());

                    float a = (float) Math.sqrt(Math.pow(right_x - left_x, 2) + Math.pow(right_y - left_y, 2));
                    float b = (float) Math.sqrt(Math.pow(right_x - nose_x, 2) + Math.pow(right_y - nose_y, 2));
                    float c = (float) Math.sqrt(Math.pow(nose_x - left_x, 2) + Math.pow(nose_y - left_y, 2));

                    float temp = (float) (Math.pow(b, 2) + Math.pow(c, 2) - Math.pow(a, 2)) / (2 * b * c);
                    float Angle = (float) Math.acos(temp);
                    Angle = (float) (Angle * (180 / Math.PI));

//                            canvas.drawText(Angle + "", nose_x, nose_y + 10, luxPaint);
                    // http://borlandforum.com/impboard/impboard.dll?action=read&db=bcb_tip&no=880

//                    if (Math.abs(ratio) <= 0.27
//                            && (Math.abs(bottom_leye_point.getY() - left_p.getY()) > 20)
//                            && face.getLeftEyeOpenProbability() > 0.4f
//                            && Math.abs(face.getHeadEulerAngleY()) < 20 && Math.abs(face.getHeadEulerAngleZ()) < 20) {       // 태블릿

                    float len = getDistance(left_p, nose_p);
                    float ren = getDistance(right_p, nose_p);
                    float lere = getDistance(left_p, right_p);
                    float ratio = (len - ren) / lere;

                    if (Math.abs(ratio) <= 0.15 && (Math.abs(bottom_leye_point.getY() - left_p.getY()) > 10)) {   // 일반 카디뷰
//                            if (Math.abs(ratio) <= 0.30 && (Math.abs(bottom_leye_point.getY() - left_p.getY()) > 10)) {   // 일반 카디뷰
                        setIsEyeTraking(true);

                        if (Math.abs(ratio) <= 0.30
                                && (Math.abs(bottom_leye_point.getY() - left_p.getY()) > 20)
                                && face.getLeftEyeOpenProbability() > 0.4f
                                && Math.abs(face.getHeadEulerAngleY()) < 20 && Math.abs(face.getHeadEulerAngleZ()) < 20
                                && (20 <= Angle && Angle <= 135)) {       // 태블릿
                            setIsFocus(true);
                        } else {
                            setIsFocus(false);
                        }

//                            if (Math.abs(ratio) <= 0.27
//                                    && (Math.abs(bottom_leye_point.getY() - left_p.getY()) > 20)
//                                    && face.getLeftEyeOpenProbability() > 0.4f
//                                    && Math.abs(face.getHeadEulerAngleY()) < 20 && Math.abs(face.getHeadEulerAngleZ()) < 20
//                                    && (20 <= Angle && Angle <= 125)) {       // 태블릿
                        if (face.getLeftEyeOpenProbability() < 0.4f) {
                            numClose++;
                        } else {
                            numClose = 0;

                            if (bitmap != null && isSave) {
//                                if (findEyeBuffer.getTotalNum() > 180 * 5 && findEyeBuffer.size() > 50) {
                                if (findEyeBuffer.size() > 50) {
                                    try {
                                        findEyeBuffer.clear();
                                    } catch (Exception e) {
                                        e.printStackTrace();
                                    }
//                                    return;
                                }
                                if (findEyeBuffer != null) {
                                    findEyeBuffer.add(new FindEyeData(face,
                                            new RectF(deScaleX(faceRect.left), deScaleY(faceRect.top), deScaleX(faceRect.right), deScaleY(faceRect.bottom)),
                                            new RectF(front_leye_point.getX(), top_leye_point.getY(), end_leye_point.getX(), bottom_leye_point.getY()),
                                            new RectF(end_reye_point.getX(), top_reye_point.getY(), front_reye_point.getX(), bottom_reye_point.getY()),
                                            new PointF(left_p.getX(), left_p.getY()), new PointF(right_p.getX(), right_p.getY()), bitmap, isLast));
                                    bitmap = null;
                                } else {
                                    findEyeBuffer = FindEyeBuffer.getInstance();
                                }

                            }
                        }
                    } else {
                        setIsEyeTraking(false);
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
            Log.e(TAG, e.getMessage());
        }
//            }
//        };
//        handler.sendEmptyMessage(0);
    }

    private float getRotatedY(FirebaseVisionPoint p1, FirebaseVisionPoint p2, float x, float offset) {
        return (((p1.getX() - p2.getX()) / (p1.getX() - p2.getX())) * (x - p1.getX())) + p1.getX() + offset;
    }


    private float getDistance(FirebaseVisionPoint p1, FirebaseVisionPoint p2) {
        return (float) Math.sqrt(Math.pow(p1.getX() - p2.getX(), 2) + Math.pow(p1.getY() - p2.getY(), 2));
    }

}
