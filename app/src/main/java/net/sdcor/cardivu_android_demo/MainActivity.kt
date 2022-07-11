package net.sdcor.cardivu_android_demo

import android.Manifest
import android.content.SharedPreferences
import android.util.Log
import android.view.MenuItem
import android.view.View
import android.widget.ProgressBar
import androidx.fragment.app.FragmentPagerAdapter
import androidx.viewpager.widget.ViewPager.OnPageChangeListener
import com.google.android.material.bottomnavigation.BottomNavigationView
import net.sdcor.cardivu_android_demo.base.OnlyPermission
import net.sdcor.cardivu_android_demo.databinding.ActivityMainBinding
import net.sdcor.cardivu_android_demo.fragment.*

import java.lang.Exception
import java.util.*
import kotlin.collections.ArrayList


class MainActivity : BaseActivity<ActivityMainBinding>(R.layout.activity_main) {
    val TAG: String = this.javaClass.simpleName

    var pager: ViewPagerFixed? = null
    var bottomNavigationView: BottomNavigationView? = null
    var adapter: BottomMenuFragmentAdapter? = null
    var menu_home: HomeFragment? = null
    var menu_measure: MeasureFragment? = null

    var progress_main: ProgressBar? = null

    companion object {
        val requestCode_systemPermission = 12041

        var mOnlyPermission: OnlyPermission? = null
        var PERMISSIONS_Camera = ArrayList<String>()
    }

    override fun init() {
        super.init()

        PERMISSIONS_Camera.add(Manifest.permission.CAMERA)

        mOnlyPermission = OnlyPermission(activity = this, context = this)

        adapter = BottomMenuFragmentAdapter(
            supportFragmentManager,
            FragmentPagerAdapter.BEHAVIOR_RESUME_ONLY_CURRENT_FRAGMENT
        )
        pager = findViewById(R.id.bottom_menu_viewpager)
        pager?.setOffscreenPageLimit(3)
        pager?.addOnPageChangeListener(object : OnPageChangeListener {
            override fun onPageScrolled(
                position: Int,
                positionOffset: Float,
                positionOffsetPixels: Int
            ) {
            }

            override fun onPageSelected(position: Int) {
                when (position) {
                    0 -> bottomNavigationView?.getMenu()?.findItem(R.id.menu_today)
                        ?.setChecked(true)
                    1 -> bottomNavigationView?.getMenu()
                        ?.findItem(R.id.menu_analysis)?.setChecked(true)
                }
            }

            override fun onPageScrollStateChanged(state: Int) {}
        })
        bottomNavigationView =
            findViewById<BottomNavigationView>(R.id.bottom_navigation)
        bottomNavigationView?.setOnNavigationItemSelectedListener(
            BottomNavigationView.OnNavigationItemSelectedListener { menuItem: MenuItem ->
                changeBottomMenu(menuItem.itemId)
                false
            })

        // 홈 화면
        if (menu_home == null) {
            menu_home = HomeFragment()
        }
        adapter!!.addItem(menu_home!!)

        // 측정 화면
        if (menu_measure == null) {
            menu_measure = MeasureFragment()
        }
        adapter!!.addItem(menu_measure!!)

        pager?.setAdapter(adapter)
        bottomNavigationView?.getMenu()?.findItem(R.id.menu_today)?.setChecked(true)
        progress_main = findViewById(R.id.progress_main)
        progress_main!!.visibility = View.GONE
    }

    override fun onDestroy() {
        if (menu_measure != null) {
            try {
                menu_measure!!.stopMEASURE()
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }

        super.onDestroy()
    }

    override fun onResume() {
        super.onResume()
        Log.e(TAG, "onResume")
    }

    override fun onBackPressed() {
        if (pager?.getCurrentItem()!! > 0)
            changeBottomMenu(R.id.menu_today)
        else
            super.onBackPressed()
    }

    fun changeBottomMenu(id: Int) {
        when (id) {
            R.id.menu_today -> pager?.setCurrentItem(0, true)
            R.id.menu_analysis -> pager?.setCurrentItem(1, true)
        }
    }
}