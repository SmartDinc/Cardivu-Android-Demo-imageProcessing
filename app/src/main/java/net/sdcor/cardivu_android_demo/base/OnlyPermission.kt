/* ff-1204 OnlyPermission [ https://github.com/ff-1204/OnlyPermission ]
MIT License

Copyright (c) 2021 ff-1204

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
package net.sdcor.cardivu_android_demo.base

import android.app.Activity
import android.content.Context
import android.content.pm.PackageManager
import android.util.Log
import androidx.core.app.ActivityCompat
import net.sdcor.cardivu_android_demo.MainActivity
import kotlin.collections.ArrayList

class OnlyPermission(protected var activity: Activity, protected var context: Context) {
    val TAG = this.javaClass.simpleName

    private val requestCode_systemPermission = 12041


    fun checkSystemPermission(
        PERMISSIONS: ArrayList<String>,
        isOnlyPermissionCheck: Boolean,
        requestCode: Int
    ): Boolean {
        val permissionList = ArrayList<String>()
        permissionList.clear()
        for (permission in PERMISSIONS) {
            if (ActivityCompat.checkSelfPermission(
                    context,
                    permission
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                permissionList.add(permission)
            }
        }
        return if (permissionList.size > 0) {
            val array = arrayOfNulls<String>(permissionList.size)
            var size = 0
            for (temp in permissionList) {
                array[size++] = temp
            }
            var isPermission = true
            for (permission in array) {
                if (ActivityCompat.checkSelfPermission(
                        context,
                        permission!!
                    ) != PackageManager.PERMISSION_GRANTED
                ) {
                    isPermission = isPermission && false
                }
            }
            if (!isPermission) {
                if (!isOnlyPermissionCheck) {
                    ActivityCompat.requestPermissions(activity, array, requestCode)
                }
            }
            false
        } else {
            true
        }
    }


    fun CheckPermission_Camera(): Boolean {
        return if (MainActivity.mOnlyPermission!!.checkSystemPermission(
                MainActivity.PERMISSIONS_Camera,
                true,
                requestCode_systemPermission
            )
        ) {
            Log.e(TAG, "권한 허용");
            true
        } else {
            Log.e(TAG, "권한 비허용")
            false
        }
    }
}