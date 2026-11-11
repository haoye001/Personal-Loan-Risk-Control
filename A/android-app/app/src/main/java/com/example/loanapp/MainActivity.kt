package com.example.loanapp

import android.content.Intent
import android.os.Bundle
import android.view.inputmethod.EditorInfo
import android.widget.Button
import android.widget.EditText
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat


class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_main)

        // 登录逻辑抽出来，按钮和回车都能调
        fun doLogin() {
            val phone = findViewById<EditText>(R.id.editTextPhone).text.toString()
            val password = findViewById<EditText>(R.id.editTextTextPassword).text.toString()

            if (phone.isEmpty()) {
                Toast.makeText(this, "请输入手机号", Toast.LENGTH_SHORT).show()
            } else if (password.isEmpty()) {
                Toast.makeText(this, "请输入密码", Toast.LENGTH_SHORT).show()
            } else {
                val intent = Intent(this, HomeActivity::class.java)
                intent.putExtra("phone", phone)
                startActivity(intent)
            }
        }

        // 点按钮登录
        findViewById<Button>(R.id.button).setOnClickListener {
            doLogin()
        }

        // 密码框按回车键（键盘上的"完成/前往"）也登录
        findViewById<EditText>(R.id.editTextTextPassword).setOnEditorActionListener { _, actionId, _ ->
            if (actionId == EditorInfo.IME_ACTION_DONE) {
                doLogin()
                true
            } else {
                false
            }
        }

        // 跳注册页
        findViewById<Button>(R.id.buttonRegister).setOnClickListener {
            startActivity(Intent(this, RegisterActivity::class.java))
        }

        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }
    }
}
