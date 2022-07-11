package net.sdcor.facemesh.retrofit

import com.jakewharton.retrofit2.converter.kotlinx.serialization.asConverterFactory
import kotlinx.serialization.json.Json
import okhttp3.MediaType
import okhttp3.OkHttpClient
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory
import java.util.concurrent.TimeUnit

object RetrofitDataSource {
    private var retrofit: Retrofit? = null

    private const val BASE_URL = "https://www.cardivu.com/"

    fun getInstance(): Retrofit? {
        if (retrofit == null) {
            val okHttpClient = OkHttpClient.Builder()
                .connectTimeout(2, TimeUnit.MINUTES) // 연결 타임아웃
                .readTimeout(30, TimeUnit.SECONDS) // 읽기 타임아웃
                .writeTimeout(15, TimeUnit.SECONDS) // 쓰기 타임아웃
                .build()


            retrofit = Retrofit.Builder()
                .baseUrl(BASE_URL)
                .client(okHttpClient)
                .addConverterFactory(
//                    Json.asConverterFactory(MediaType.parse("application/json")!!)
                    GsonConverterFactory.create()
                )
                .build()
        }
        return retrofit
    }
}