package com.example.loanapp

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import androidx.appcompat.app.AppCompatActivity
import org.json.JSONObject

class ProfileActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_profile)

        val fields = mapOf(
            "phone" to findViewById<EditText>(R.id.editProfilePhone),
            "age" to findViewById<EditText>(R.id.editProfileAge),
            "income" to findViewById<EditText>(R.id.editProfileIncome),
            "debt" to findViewById<EditText>(R.id.editProfileDebt),
            "occupation" to findViewById<EditText>(R.id.editProfileOccupation)
        )
        val save = findViewById<Button>(R.id.buttonSaveProfile)
        save.isEnabled = false
        findViewById<Button>(R.id.buttonLogout).setOnClickListener { returnToLogin() }

        runRequest(action = { JSONObject(LoanApi.request("/users/me")) }) { profile ->
            for ((key, field) in fields) {
                if (!profile.isNull(key)) field.setText(profile.getString(key))
            }
            save.isEnabled = true
        }

        save.setOnClickListener {
            val body = JSONObject()
            for ((key, field) in fields) {
                val value = field.text.toString().trim()
                if (value.isEmpty() && key != "occupation") {
                    showMessage("请填写手机号、年龄、月收入和负债")
                    return@setOnClickListener
                }
                body.put(key, if (value.isEmpty()) JSONObject.NULL else value)
            }
            save.isEnabled = false
            runRequest(action = { LoanApi.request("/users/me", "PUT", body) },
                onComplete = { save.isEnabled = true }) {
                showMessage("资料已保存")
                finish()
            }
        }
    }
}
