package net.sdcor.sdapi.imageprocmodule;

import android.util.Log;

import androidx.annotation.NonNull;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.OnSuccessListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.ml.vision.FirebaseVision;
import com.google.firebase.ml.vision.common.FirebaseVisionImage;
import com.google.firebase.ml.vision.face.FirebaseVisionFace;
import com.google.firebase.ml.vision.face.FirebaseVisionFaceDetector;
import com.google.firebase.ml.vision.face.FirebaseVisionFaceDetectorOptions;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

public class ModuleLinkedCamera {

    private final String TAG = this.getClass().getSimpleName();

    /**
     * camera 개발자
     * preview setting 개발자
     * 얼굴 찾기 MLkit API
     * 눈 좌표 개발자 API
     * 얼굴, 눈 그리기 개발자
     * <p>
     * camera 시작시 imageProcessing 시작시키게?
     * camera 에서 받은 이미지 API 로 전송 (1초에 최소 5장? 이미지 전송)→ 1초간 계산후 GQ count 전송
     * 이미지랑 좌표 같이 받아야함.(얼굴, 오른쪽 눈) 리턴값으로 홍채 중심 좌표, 옵티컬플로우, 반지름
     * 옵티컬 플로우 서버에 보내기
     * 1초에 한 번씩 GQ Count  호출.
     **/

    public static class FaceImage {
        private FirebaseVisionImage image;
        private boolean last;

        FaceImage(FirebaseVisionImage image) {
            this.image = image;
            this.last = false;
        }

        FirebaseVisionImage getImage() {
            return image;
        }

        boolean isLast() {
            return last;
        }

        void setLast() {
            this.last = true;
        }
    }

    private ArrayList<ArrayList<FaceImage>> imagesBuffer;
    private ArrayList<FaceImage> tempImagesBuffer;
    private FaceGraphic faceGraphic;
    private FirebaseVisionFaceDetector detector;
    private GraphicOverlay graphicOverlay;

    private AtomicBoolean isAnalyzing;
    private AtomicBoolean isDetecting;

    private int minFrame = 7;
    private int frameCnt = 0;
    private ScheduledExecutorService detectingService = Executors.newSingleThreadScheduledExecutor();
    private boolean isStartFaceDetect = false;
    private long firstTime = -1;


    public void initialFunction(GraphicOverlay graphicOverlay) {
        faceGraphic = new FaceGraphic(graphicOverlay);
        imagesBuffer = new ArrayList<>();
        tempImagesBuffer = new ArrayList<>();
        isStartFaceDetect = false;

        /* FireBase FaceDetector Setting */
        FirebaseVisionFaceDetectorOptions visionRealtimeOptions = new FirebaseVisionFaceDetectorOptions.Builder()
                .setPerformanceMode(FirebaseVisionFaceDetectorOptions.FAST)
                .setLandmarkMode(FirebaseVisionFaceDetectorOptions.ALL_LANDMARKS)
                .setClassificationMode(FirebaseVisionFaceDetectorOptions.ALL_CLASSIFICATIONS) //눈 감음
                .setMinFaceSize(0.5f)
                .enableTracking()
                .build();

        detector = FirebaseVision.getInstance().getVisionFaceDetector(visionRealtimeOptions);
        this.graphicOverlay = graphicOverlay;

        isAnalyzing = new AtomicBoolean(false);
        isDetecting = new AtomicBoolean(false);
        frameCnt = 0;
    }


    public void setFrameNum(int num) {
        minFrame = num;
    }

    public void inputImage(@NonNull FirebaseVisionImage image) {
        int interval = (1000 / minFrame);
        long inputTime = System.currentTimeMillis();
        if (firstTime < 0) firstTime = inputTime;
        if (inputTime - firstTime >= 1000) frameCnt = 0;
        try {
            try {
                if (frameCnt == 0) {
                    firstTime = inputTime;
                    if (tempImagesBuffer.size() > 0) {
                        FaceImage faceImage = tempImagesBuffer.get(tempImagesBuffer.size() - 1);
                        faceImage.setLast();
                        imagesBuffer.get(imagesBuffer.size() - 1).add(faceImage);
                    }
                    tempImagesBuffer.clear();
                    tempImagesBuffer.add(new FaceImage(image));

                    imagesBuffer.add(new ArrayList<FaceImage>());
                    imagesBuffer.get(imagesBuffer.size() - 1).add(new FaceImage(image));
                } else {
                    long t_diff = inputTime - firstTime;
                    int tempidx = (int) t_diff / interval;
                    if (tempidx > 0) {
                        if (tempImagesBuffer.size() == tempidx) {
                            tempImagesBuffer.add(new FaceImage(image));
                        } else if (tempImagesBuffer.size() > tempidx) {
                            tempImagesBuffer.set(tempidx, new FaceImage(image));
                        } else {
                            FaceImage lastFaceimg = tempImagesBuffer.get(tempImagesBuffer.size() - 1);
                            for (int i = tempImagesBuffer.size(); i < tempidx; i++) {
                                tempImagesBuffer.add(null);
                            }
                            tempImagesBuffer.add(new FaceImage(image));
                        }
                        int bufferSize = imagesBuffer.size();
                        int asize = imagesBuffer.get(bufferSize - 1).size();
                        if (tempidx > 1 && asize == tempidx - 1) {
                            imagesBuffer.get(bufferSize - 1).add(tempImagesBuffer.get(tempidx - 1));
                        } else if (tempidx > 1 && asize < tempidx - 1) {
                            for (int i = asize; i <= tempidx - 1; i++) {
                                imagesBuffer.get(imagesBuffer.size() - 1).add(tempImagesBuffer.get(i));
                            }
                        }
                    }
                }
            } catch (Exception e) {
                Log.e(TAG, "inputImage: " + e.getMessage());
                e.printStackTrace();
            } finally {
                isAnalyzing.set(false);
            }

            if (!isStartFaceDetect && imagesBuffer.size() > 0) {
                isStartFaceDetect = true;
                if (detectingService != null) {
                    try {
                        detectingService.scheduleAtFixedRate(detectingRun, 0, 30, TimeUnit.MILLISECONDS);
                    } catch (Exception e) {
                        Log.e(TAG, "inputImage: " + e.getMessage());
                        e.printStackTrace();
                    }
                } else {
                    detectingService = Executors.newSingleThreadScheduledExecutor();
                    try {
                        detectingService.scheduleAtFixedRate(detectingRun, 0, 30, TimeUnit.MILLISECONDS);
                    } catch (Exception e) {
                        Log.e(TAG, "inputImage: " + e.getMessage());
                        e.printStackTrace();
                    }
                }
            }
            frameCnt++;
        } catch (Exception e) {
            Log.e(TAG, "inputImage: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private Runnable detectingRun = new Runnable() {
        int sec = 0;
        int num = 0;

        @Override
        public void run() {
            if (isStartFaceDetect) {
                if (isDetecting.get()) {
                    return;
                }
                isDetecting.set(true);
                try {
                    ArrayList<FaceImage> arrayList = imagesBuffer.get(sec);
                    if (arrayList.size() > num) {
                        FaceImage faceImage = arrayList.get(num);
                        if (faceImage == null) {
                            num++;
                            isDetecting.set(false);
                            return;
                        }
                        final FirebaseVisionImage visionImage = faceImage.getImage();
                        final boolean islast = faceImage.isLast();
                        if (visionImage != null) {
                            detector.detectInImage(visionImage).addOnSuccessListener(new OnSuccessListener<List<FirebaseVisionFace>>() {
                                //                                int N = num;
                                @Override
                                public void onSuccess(List<FirebaseVisionFace> firebaseVisionFaces) {
                                    try {
                                        if (graphicOverlay != null) {
                                            graphicOverlay.clear();
                                        }
                                        if (firebaseVisionFaces.size() != 0) {
                                            FirebaseVisionFace visionFace = null;
                                            for (FirebaseVisionFace face : firebaseVisionFaces) {
                                                if (visionFace == null) visionFace = face;
                                                else {
                                                    int vsionwh = visionFace.getBoundingBox().width() * visionFace.getBoundingBox().height();
                                                    int facewh = face.getBoundingBox().width() * face.getBoundingBox().height();
                                                    if (vsionwh < facewh) visionFace = face;
                                                }
                                            }
                                            if (visionFace != null && visionImage != null) {
                                                faceGraphic.setData(visionFace, visionImage.getBitmap(), islast);
                                            }
                                            if (graphicOverlay != null) {
                                                graphicOverlay.clear();
                                                if (faceGraphic != null) {
                                                    graphicOverlay.add(faceGraphic);
                                                }
                                            }
                                        }
                                    } catch (Exception e) {
                                        e.printStackTrace();
                                    }
                                }
                            }).addOnCompleteListener(new OnCompleteListener<List<FirebaseVisionFace>>() {
                                @Override
                                public void onComplete(@NonNull Task<List<FirebaseVisionFace>> task) {
                                    isDetecting.set(false);
                                    num++;
                                }
                            });
                        } else {
                            isDetecting.set(false);
                        }
                    } else if (arrayList.size() > 0 && arrayList.size() == num) {
                        if (imagesBuffer.size() > sec + 1) {
                            try {
                                imagesBuffer.set(sec, null);
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                            sec += 1;
                            num = 0;
                        }
                        isDetecting.set(false);
                    } else if (num > minFrame) {
                        num = 0;
                        isDetecting.set(false);
                    }
                } catch (Exception e) {
                    Log.e(TAG, "detectingRun: " + e.getMessage());
                    e.printStackTrace();
                } finally {
                    try {
                        if (sec >= 2 && imagesBuffer.get(sec - 2) != null) {
                            imagesBuffer.set(sec - 2, null);
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            } else {
                if (detectingService != null) {
                    try {
                        detectingService.shutdown();
                        detectingService = null;
                    } catch (Exception e) {
                        Log.e(TAG, "detectingRun: " + e.getMessage());
                        e.printStackTrace();
                    }
                }
            }
        }
    };
}
