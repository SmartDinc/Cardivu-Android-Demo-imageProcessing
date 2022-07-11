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
import android.widget.Button
import android.widget.ProgressBar
import android.widget.TextView
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.fragment.app.Fragment
import net.sdcor.cardivu_android_demo.MainActivity
import net.sdcor.cardivu_android_demo.databinding.FragmentMeasureBinding
import net.sdcor.cardivu_android_demo.sdapi.CameraManager
import net.sdcor.facemesh.retrofit.ReadyService
import net.sdcor.facemesh.retrofit.RetrofitDataSource
import net.sdcor.sdapi.imageprocmodule.HeartDataListener
import net.sdcor.sdapi.imageprocmodule.ProcessingThread
import net.sdcor.sdapi.server.SD_ServerAlgorithmus
import org.json.JSONObject
import java.text.SimpleDateFormat

class MeasureFragment : Fragment() {
    val TAG: String = this.javaClass.simpleName
    val timeFormat = SimpleDateFormat("yyyy-MM-dd HH:mm:ss")

    val CompanyCode = "CompanyCode"             // 회사코드
    val CompanyKey = "CompanyKey"               // 회사 인증키
    val CompanyClient = "CompanyClient_Key"     // 회사의 회원별 고유키
    var START_IDX = 0                                   // 첫 시작시 홍채 변수 IDX
    var LAST_IDX = 0                                    // 마지막 홍채 변수 IDX
    val MeasureTime = 300                               // 총 측정할 시간
    var lastBPM = 0f
    var CountIDX = 0

    private var progressBarBpm: ProgressBar? = null
    private var textViewProgressBpm: TextView? = null
    private var textViewBpmValue: TextView? = null

    private var measureBtn: Button? = null
    private var measure_top: ConstraintLayout? = null
    private var mCameraManager: CameraManager? = null

    var mHeartDataListener: HeartDataListener? = null

    var mSD_ServerAlgorithmus: SD_ServerAlgorithmus? = null
    private var processingThread: ProcessingThread? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Log.d(TAG, "onCreate")

        mSD_ServerAlgorithmus = SD_ServerAlgorithmus()
    }

    private var primaryBaseActivity
            : Context? = null

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

        val binding = FragmentMeasureBinding.inflate(inflater, container, false)

        measureBtn = binding.measureBtn
        measure_top = binding.measureTop
        progressBarBpm = binding.progressBarBpm
        textViewProgressBpm = binding.textViewProgressBpm
        textViewBpmValue = binding.textViewBpmValue


        binding.measureBtn.setOnClickListener {
            if (MainActivity.mOnlyPermission!!.CheckPermission_Camera()) {
                Thread {
                    requireActivity().runOnUiThread {
                        measureBtn!!.visibility = View.INVISIBLE
                    }
                }.start()

                mHeartDataListener = object : HeartDataListener {
                    override fun onGetData(
                        ResultText: String
                    ) {
                        try {
                            if (ResultText != null && ResultText != "") {
//                                Log.d(TAG, "ResultText: " + ResultText)

                                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) { //23버전부터 사용가능
                                    val connManager =
                                        context!!.getSystemService(Service.CONNECTIVITY_SERVICE) as ConnectivityManager
                                    val network: Network? = connManager.activeNetwork
                                    if (network != null) {
                                        try {
                                            var service = RetrofitDataSource.getInstance()!!
                                                .create(ReadyService::class.java)
                                            val response =
                                                service!!.measure(
                                                    CompanyCode = CompanyCode,
                                                    CompanyKey = CompanyKey,
                                                    CompanyClient = CompanyClient,
                                                    date = timeFormat.format(System.currentTimeMillis()),
                                                    data = ResultText,
                                                    START_IDX = START_IDX
                                                )?.execute()
                                            if (response!!.isSuccessful) {
//                                                Log.e(TAG, response.toString())
//                                                Log.e(TAG, response.body()!!.string())

                                                val ResultJson =
                                                    JSONObject(response.body()!!.string())

                                                if (START_IDX == 0) {
                                                    START_IDX = ResultJson.getInt("LAST_IDX");
                                                }
                                                LAST_IDX = ResultJson.getInt("LAST_IDX");
                                                CountIDX = ResultJson.getInt("CountIDX");
                                                if (CountIDX > 60) {
                                                    lastBPM = ResultJson.getDouble("BPM").toFloat()
                                                }
//                                                Log.e(TAG, ResultJson.toString())
//                                                Log.e(TAG, "START_IDX: "+START_IDX+", LAST_IDX: "+LAST_IDX+", CountIDX: "+CountIDX+", lastBPM: "+lastBPM)
                                            }

                                        } catch (e: Exception) {
                                            e.message
                                        }
                                    }
                                }
//                                if (CountIDX >= MeasureTime) {
//                                    stopMEASURE()
//                                }

                                Thread {
                                    requireActivity().runOnUiThread {
                                        val percent =
                                            ((CountIDX.toFloat() / MeasureTime.toFloat()).toFloat() * 100).toInt()
                                        // Int로 자동 형변환 되어 toFloat를 적용해야 나누기할때 소수점이 표현된다.
                                        textViewProgressBpm!!.text = percent.toString() + " (%)"
                                        progressBarBpm!!.progress = percent
                                        textViewBpmValue!!.text = "BPM : " + lastBPM.toString()

                                        if (CountIDX >= 60) {
                                            textViewBpmValue!!.visibility = View.VISIBLE
                                        } else {
                                            textViewBpmValue!!.visibility = View.INVISIBLE
                                        }
                                        Log.e(
                                            TAG,
                                            "START_IDX: " + START_IDX + ", LAST_IDX: " + LAST_IDX + ", CountIDX: " + CountIDX + ", lastBPM: " + lastBPM + ", percent: " + percent
                                        )
                                    }
                                }.start()


                            }
                        } catch (e: Exception) {
                            e.message
                        }
                    }

                    override fun goWait60s(diffCount: Int) {}
                    override fun setLux5_low(Lux: Float) {}
                }

                mCameraManager = CameraManager(requireContext(), measure_top, false)
                processingThread =
                    ProcessingThread(
                        requireContext(),
                        mHeartDataListener,
                        mSD_ServerAlgorithmus,
                        false
                    )

                startMEASURE()
            } else {
                if (MainActivity.mOnlyPermission!!.checkSystemPermission(
                        MainActivity.PERMISSIONS_Camera,
                        false,
                        MainActivity.requestCode_systemPermission
                    )
                ) {
                }
            }
        }

        return binding!!.root
    }


    private fun startMEASURE() {
        mCameraManager!!.setUpCamera()
        processingThread!!.initialize()
        processingThread!!.startProcessing()
    }

    fun stopMEASURE() {
        if (processingThread != null) {
            try {
                processingThread!!.stopProcessing()
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
        if (mCameraManager != null) {
            try {
                mCameraManager!!.stop()
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }

    }
}