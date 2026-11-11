package com.example.loanapp

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import org.json.JSONObject

class ApplyActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_apply)
        val loanId = intent.getIntExtra("loanId", 0)
        val maxAmount = intent.getStringExtra("maxAmount").orEmpty()
        val term = intent.getIntExtra("term", 0)
        findViewById<TextView>(R.id.textProductName).text = intent.getStringExtra("productName")
        findViewById<TextView>(R.id.textProductInfo).text = "最高 $maxAmount 元，期限 $term 个月"

        val button = findViewById<Button>(R.id.buttonSubmit)
        button.setOnClickListener {
            val amount = findViewById<EditText>(R.id.editAmount).text.toString().trim().toBigDecimalOrNull()
            val limit = maxAmount.toBigDecimalOrNull()
            if (amount == null || limit == null || amount.signum() <= 0 ||
                amount.stripTrailingZeros().scale() > 2 || amount > limit) {
                showMessage("请输入有效金额，最多两位小数且不得超过产品额度")
                return@setOnClickListener
            }
            button.isEnabled = false
            val body = JSONObject().put("loanId", loanId).put("amount", amount.toPlainString())
            runRequest(action = {
                JSONObject(LoanApi.request("/applications", "POST", body))
            }, onComplete = { button.isEnabled = true }) { application ->
                val id = application.getInt("applicationId")
                val status = applicationStatus(application.getString("status"))
                showMessage("申请 #$id：$status")
                finish()
            }
        }
        findViewById<Button>(R.id.buttonBack).setOnClickListener { finish() }
    }
}
