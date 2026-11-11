package com.example.loanapp

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity

class RegisterActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_register)

        findViewById<Button>(R.id.buttonRegister).setOnClickListener {
            val phone = findViewById<EditText>(R.id.editRegPhone).text.toString()
            val pwd1 = findViewById<EditText>(R.id.editRegPassword).text.toString()
            val pwd2 = findViewById<EditText>(R.id.editRegPassword2).text.toString()

            when {
                phone.isEmpty() -> Toast.makeText(this, "请输入手机号", Toast.LENGTH_SHORT).show()
                pwd1.isEmpty() -> Toast.makeText(this, "请设置密码", Toast.LENGTH_SHORT).show()
                pwd1 != pwd2 -> Toast.makeText(this, "两次密码不一致", Toast.LENGTH_SHORT).show()
                else -> {
                    Toast.makeText(this, "注册成功，请登录", Toast.LENGTH_SHORT).show()
                    finish()
                }
            }
        }

        findViewById<Button>(R.id.buttonBackToLogin).setOnClickListener {
            finish()
        }
    }
}
