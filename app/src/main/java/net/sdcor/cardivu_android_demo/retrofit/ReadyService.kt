package net.sdcor.facemesh.retrofit

import okhttp3.ResponseBody
import retrofit2.Call
import retrofit2.http.*


interface ReadyService {
    @FormUrlEncoded
    @POST("/api/measure")
    fun measure(
        @Field("CompanyCode") CompanyCode: String,
        @Field("CompanyKey") CompanyKey: String,
        @Field("CompanyClient") CompanyClient: String,
        @Field("date") date: String,
        @Field("data") data: String,
        @Field("START_IDX") START_IDX: Int
    ): Call<ResponseBody>

    @FormUrlEncoded
    @POST("/api/select_result")
    fun select_result(
        @Field("CompanyCode") CompanyCode: String,
        @Field("CompanyKey") CompanyKey: String,
        @Field("CompanyClient") CompanyClient: String,
        @Field("Option") Option: Int,
        @Field("START_IDX") START_IDX: Int,
        @Field("LAST_IDX") LAST_IDX: Int
    ): Call<ResponseBody>
}
