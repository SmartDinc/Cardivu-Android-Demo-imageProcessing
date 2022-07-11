package net.sdcor.sdapi.imageprocmodule;

import java.util.ArrayList;

public class FindEyeBuffer {
    private static FindEyeBuffer findEyeBuffer = null;
    private ArrayList<FindEyeData> array;

    private int totalNum = 0;

    private FindEyeBuffer() {
        array = new ArrayList<>();
        array.clear();
    }

    public static FindEyeBuffer getInstance() {
        if (findEyeBuffer == null) findEyeBuffer = new FindEyeBuffer();
        return findEyeBuffer;
    }

    public void add(FindEyeData data) {
        if (array != null) {
            try {
                array.add(data);
                totalNum++;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public FindEyeData remove() {
        if (isEmpty()) return null;
        else return array.remove(0);
    }

    public boolean isEmpty() {
        return array == null || array.size() <= 0;
    }

    public int size() {
        return array.size();
    }

    public void clear() {
        if (array != null) {
            array.clear();
        }

    }

    public void init() {
        if (array != null) {
            for (int i = size() - 2; i >= 0; i--) {
                if (array.get(i).isLastFrame()) {
                    break;
                }
            }
            if (size() - 10 > 0) {
                array.subList(0, size() - 10).clear();
            }
        }
    }

    public int getTotalNum() {
        return totalNum;
    }
}