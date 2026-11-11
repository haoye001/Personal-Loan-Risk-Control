package com.example.loanapp

import android.content.Context
import android.content.Intent
import android.content.SharedPreferences
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import org.json.JSONArray
import org.json.JSONObject

internal object LoanApi {
    private const val PREFS_NAME = "loan_prefs"
    private const val KEY_TOKEN = "access_token"

    private val client = OkHttpClient()
    private val json = "application/json".toMediaType()
    private lateinit var prefs: SharedPreferences

    @Volatile var token: String? = null

    fun init(context: Context) {
        prefs = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
        token = prefs.getString(KEY_TOKEN, null)
    }

    fun saveToken(value: String) {
        token = value
        prefs.edit().putString(KEY_TOKEN, value).apply()
    }

    fun clearToken() {
        token = null
        if (::prefs.isInitialized) prefs.edit().remove(KEY_TOKEN).apply()
    }

    class Failure(val status: Int, message: String) : Exception(message)

    fun request(path: String, method: String = "GET", payload: JSONObject? = null, token: String? = this.token): String {
        val request = Request.Builder()
            .url(BuildConfig.LOAN_API_URL.trimEnd('/') + "/api" + path)
            .method(method, payload?.toString()?.toRequestBody(json))
        if (token != null) request.header("Authorization", "Bearer $token")
        return client.newCall(request.build()).execute().use { response ->
            val body = response.body?.string().orEmpty()
            if (!response.isSuccessful) {
                val detail = runCatching { JSONObject(body).opt("detail") }.getOrNull()
                val message = when (detail) {
                    is JSONArray -> (0 until detail.length()).joinToString("；") { index ->
                        val error = detail.getJSONObject(index)
                        val location = error.getJSONArray("loc")
                        "${location.getString(location.length() - 1)}：${error.getString("msg")}"
                    }
                    is String -> detail
                    else -> "请求失败（HTTP ${response.code}）"
                }
                throw Failure(response.code, message)
            }
            body
        }
    }

    fun products(): List<LoanProduct> {
        val items = JSONArray(request("/loans"))
        return List(items.length()) { index ->
            val product = items.getJSONObject(index)
            LoanProduct(product.getInt("id"), product.getString("name"),
                product.getString("maxAmount"), product.getInt("term"))
        }
    }
}

internal fun AppCompatActivity.showMessage(message: String) {
    Toast.makeText(this, message, Toast.LENGTH_LONG).show()
}

internal fun AppCompatActivity.returnToLogin() {
    LoanApi.clearToken()
    startActivity(Intent(this, MainActivity::class.java).apply {
        flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
    })
    finish()
}

internal fun <T> AppCompatActivity.runRequest(
    authenticated: Boolean = true,
    action: () -> T,
    onComplete: () -> Unit = {},
    onSuccess: (T) -> Unit
) {
    val session = LoanApi.token
    if (authenticated && session == null) {
        returnToLogin()
        return
    }
    Thread {
        val result = runCatching(action)
        runOnUiThread {
            if (isFinishing || isDestroyed) return@runOnUiThread
            if (authenticated && session != LoanApi.token) return@runOnUiThread
            onComplete()
            result.fold(onSuccess) { error ->
                if (authenticated && error is LoanApi.Failure && error.status == 401) {
                    showMessage("登录已过期，请重新登录")
                    returnToLogin()
                } else {
                    showMessage(if (error is LoanApi.Failure) error.message.orEmpty() else "无法连接或读取服务器响应")
                }
            }
        }
    }.start()
}
