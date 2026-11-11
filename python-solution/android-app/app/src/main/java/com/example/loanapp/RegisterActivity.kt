package com.example.loanapp

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import androidx.appcompat.app.AppCompatActivity
import org.json.JSONObject

class RegisterActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_register)

        val button = findViewById<Button>(R.id.buttonRegister)
        button.setOnClickListener {
            val name = findViewById<EditText>(R.id.editRegUsername).text.toString().trim()
            val phone = findViewById<EditText>(R.id.editRegPhone).text.toString().trim()
            val password = findViewById<EditText>(R.id.editRegPassword).text.toString()
            val confirmation = findViewById<EditText>(R.id.editRegPassword2).text.toString()
            if (name.isEmpty() || phone.isEmpty() || password.isEmpty() || password != confirmation) {
                showMessage("请填写用户名、手机号和密码，并确认两次密码一致")
                return@setOnClickListener
            }
            button.isEnabled = false
            val body = JSONObject().put("username", name).put("phone", phone).put("password", password)
            runRequest(authenticated = false, action = {
                LoanApi.request("/auth/register", "POST", body, token = null)
            }, onComplete = { button.isEnabled = true }) {
                showMessage("注册成功，请登录")
                finish()
            }
        }
        findViewById<Button>(R.id.buttonBackToLogin).setOnClickListener { finish() }
    }
}
