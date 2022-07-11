package net.sdcor.cardivu_android_demo.fragment

import android.app.Service
import android.content.Context
import android.net.ConnectivityManager
import android.net.Network
import android.os.Build
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.fragment.app.Fragment
import net.sdcor.cardivu_android_demo.MainActivity
import net.sdcor.cardivu_android_demo.databinding.FragmentHomeBinding
import net.sdcor.facemesh.retrofit.ReadyService
import net.sdcor.facemesh.retrofit.RetrofitDataSource
import org.json.JSONObject

class HomeFragment : Fragment() {
    val TAG: String = this.javaClass.simpleName

    val CompanyCode = "CompanyCode"             // 회사코드
    val CompanyKey = "CompanyKey"               // 회사 인증키
    val CompanyClient = "CompanyClient_Key"     // 회사의 회원별 고유키

    private var textViewTime: TextView? = null
    private var textViewBpm: TextView? = null
    private var textViewSdnn: TextView? = null
    private var textViewStress: TextView? = null

    private var buttonIsPerm: TextView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Log.d(TAG, "onCreate")
    }

    private var primaryBaseActivity: Context? = null
    override fun onAttach(context: Context) {
        primaryBaseActivity = context
        super.onAttach(context)
        Log.d(TAG, "ProfileFragment - onAttach() called")
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        Log.d(TAG, "onCreateView")

        val binding = FragmentHomeBinding.inflate(inflater, container, false)

        textViewTime = binding.textViewTime
        textViewBpm = binding.textViewBpm
        textViewSdnn = binding.textViewSdnn
        textViewStress = binding.textViewStress

        buttonIsPerm = binding.buttonIsPerm

        Thread {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) { //23버전부터 사용가능
                val connManager =
                    primaryBaseActivity!!.getSystemService(Service.CONNECTIVITY_SERVICE) as ConnectivityManager
                val network: Network? = connManager.activeNetwork
                if (network != null) {
                    try {
                        var service = RetrofitDataSource.getInstance()!!
                            .create(ReadyService::class.java)
                        val response =
                            service!!.select_result(
                                CompanyCode = CompanyCode,
                                CompanyKey = CompanyKey,
                                CompanyClient = CompanyClient,
                                Option = 0,
                                START_IDX = -1,
                                LAST_IDX = -1
                            )?.execute()
                        if (response!!.isSuccessful) {
//                            Log.e(TAG, response.toString())
//                            Log.e(TAG, response.body()!!.string())

                            val responseStr =
                                JSONObject(response.body()!!.string()).getString("Data")

                            if (responseStr.length > 5) {
                                val ResultJson =
                                    JSONObject(responseStr.substring(1 until responseStr.length - 1))
//                                Log.e(TAG, ResultJson.toString())
                                requireActivity().runOnUiThread {
                                    try {
                                        textViewTime!!.text =
                                            ResultJson.getString("MEASURE_END_TIME")
                                        textViewBpm!!.text =
                                            getMathAround(ResultJson.getDouble("BPM"), 1).toString()
                                        textViewSdnn!!.text =
                                            getMathAround(
                                                ResultJson.getDouble("SDNN"),
                                                2
                                            ).toString()

                                        val stress =
                                            getMathAround(ResultJson.getDouble("STRESS"), 1)

                                        if (stress > 80) {
                                            textViewStress!!.text =
                                                "매우 높음(" + stress.toString() + "점)"
                                        } else if (ResultJson.getInt("STRESS") > 60) {
                                            textViewStress!!.text = "높음(" + stress.toString() + "점)"
                                        } else if (ResultJson.getInt("STRESS") > 40) {
                                            textViewStress!!.text = "보통(" + stress.toString() + "점)"
                                        } else if (stress > 20) {
                                            textViewStress!!.text = "낮음(" + stress.toString() + "점)"
                                        } else {
                                            textViewStress!!.text =
                                                "매우 낮음(" + stress.toString() + "점)"
                                        }
                                    } catch (e: Exception) {
                                        e.message
                                    }
                                }
                            }
                        }
                    } catch (e: Exception) {
                        e.message
                    }
                }
            }

            try {
                if (MainActivity.mOnlyPermission!!.CheckPermission_Camera()) {
                    buttonIsPerm!!.visibility = View.VISIBLE
                }
            } catch (e: Exception) {
                e.message
            }

        }.start()

        return binding!!.root
    }

    fun getMathAround(targetValue: Double, position: Int): Double {
        val ndb = Math.pow(10.0, position.toDouble())
        return Math.round(targetValue * ndb) / ndb
    }
}