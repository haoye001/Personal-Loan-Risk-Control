package com.example.loanapp

import android.content.Intent
import android.os.Bundle
import android.view.inputmethod.EditorInfo
import android.widget.Button
import android.widget.EditText
import androidx.appcompat.app.AppCompatActivity
import org.json.JSONObject

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        LoanApi.init(this)
        // 已保存登录态则直接进入首页，免重复登录
        if (LoanApi.token != null) {
            startActivity(Intent(this, HomeActivity::class.java))
            finish()
            return
        }

        setContentView(R.layout.activity_main)
        val login = findViewById<Button>(R.id.button)
        val register = findViewById<Button>(R.id.buttonRegister)
        val username = findViewById<EditText>(R.id.editTextUsername)
        val password = findViewById<EditText>(R.id.editTextTextPassword)

        fun signIn() {
            if (!login.isEnabled) return
            val name = username.text.toString().trim()
            val secret = password.text.toString()
            if (name.isEmpty() || secret.isEmpty()) {
                showMessage("请输入用户名和密码")
                return
            }
            login.isEnabled = false
            register.isEnabled = false
            runRequest(authenticated = false, action = {
                val body = JSONObject().put("username", name).put("password", secret)
                JSONObject(LoanApi.request("/auth/login", "POST", body, token = null)).getString("access_token")
            }, onComplete = {
                login.isEnabled = true
                register.isEnabled = true
            }) { token ->
                LoanApi.saveToken(token)
                startActivity(Intent(this, HomeActivity::class.java))
                finish()
            }
        }

        login.setOnClickListener { signIn() }
        register.setOnClickListener { startActivity(Intent(this, RegisterActivity::class.java)) }
        password.setOnEditorActionListener { _, action, _ ->
            if (action == EditorInfo.IME_ACTION_DONE) {
                signIn()
                true
            } else false
        }
    }
}
