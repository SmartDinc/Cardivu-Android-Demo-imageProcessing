package net.sdcor.cardivu_android_demo.fragment

import android.content.Context
import android.util.AttributeSet
import android.util.Log
import android.view.MotionEvent
import androidx.viewpager.widget.ViewPager


class ViewPagerFixed : ViewPager {
    companion object {
        val TAG: String = this.javaClass.simpleName
    }

    constructor(context: Context?) : super(context!!) {}
    constructor(context: Context?, attrs: AttributeSet?) : super(context!!, attrs) {}

    override fun onTouchEvent(ev: MotionEvent?): Boolean {
        try {
            return super.onTouchEvent(ev)
        } catch (ex: Exception) {
            Log.d(TAG, ex.message.toString())
        }
        return false
    }

    override fun onInterceptTouchEvent(ev: MotionEvent?): Boolean {
        try {
            return super.onInterceptTouchEvent(ev)
        } catch (ex: Exception) {
            Log.d(TAG, ex.message.toString())
        }
        return false
    }
}