package com.example.loanapp

import android.os.Bundle
import android.widget.Button
import android.widget.EditText
import android.widget.TextView
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity

class ApplyActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_apply)

        supportActionBar?.title = "贷款申请"
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        // 从首页传过来的产品信息
        val productName = intent.getStringExtra("productName") ?: ""
        val maxAmount = intent.getIntExtra("maxAmount", 0)
        val rate = intent.getDoubleExtra("rate", 0.0)

        // 显示产品信息
        findViewById<TextView>(R.id.textProductName).text = productName
        findViewById<TextView>(R.id.textProductInfo).text =
            "最高 $maxAmount 元，日利率 $rate%"

        // 提交按钮
        findViewById<Button>(R.id.buttonSubmit).setOnClickListener {
            val amount = findViewById<EditText>(R.id.editAmount).text.toString()
            val months = findViewById<EditText>(R.id.editMonths).text.toString()

            if (amount.isEmpty()) {
                Toast.makeText(this, "请输入借款金额", Toast.LENGTH_SHORT).show()
            } else if (months.isEmpty()) {
                Toast.makeText(this, "请输入借款期限", Toast.LENGTH_SHORT).show()
            } else {
                Toast.makeText(this, "申请成功！金额 $amount 元，期限 $months 个月", Toast.LENGTH_LONG).show()
                finish() // 关掉申请页，回到列表
            }
        }

        findViewById<android.widget.Button>(R.id.buttonBack).setOnClickListener {
            finish()
        }
    }

    override fun onOptionsItemSelected(item: android.view.MenuItem): Boolean {
        if (item.itemId == android.R.id.home) {
            finish()
            return true
        }
        return super.onOptionsItemSelected(item)
    }
}
