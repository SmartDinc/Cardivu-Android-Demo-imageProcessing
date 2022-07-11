package net.sdcor.cardivu_android_demo.fragment

import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentManager
import androidx.fragment.app.FragmentPagerAdapter
import java.util.ArrayList


class BottomMenuFragmentAdapter internal constructor(fm: FragmentManager, behavior: Int) :
    FragmentPagerAdapter(fm, behavior) {
    private val fragments = ArrayList<Fragment>()
    override fun getCount(): Int {
        return fragments.size
    }

    override fun getItem(position: Int): Fragment {
        return fragments[position]
    }

    fun addItem(fragment: Fragment) {
        fragments.add(fragment)
    }
}
