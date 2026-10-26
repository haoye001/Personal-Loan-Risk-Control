package com.example.loanapp

import android.os.Bundle
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView

class MyLoansActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_my_loans)

        // 顶部标题 + 返回箭头
        supportActionBar?.title = "我的贷款"
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        // 写死几条假记录
        val fakeMyLoans = listOf(
            MyLoan("极速贷", 10000, 12, "审核中"),
            MyLoan("工薪贷", 50000, 24, "已放款"),
            MyLoan("车主贷", 80000, 36, "已拒绝")
        )

        val recyclerView = findViewById<RecyclerView>(R.id.recyclerViewMyLoans)
        recyclerView.layoutManager = LinearLayoutManager(this)
        recyclerView.adapter = MyLoanAdapter(fakeMyLoans)

        findViewById<android.widget.Button>(R.id.buttonBack).setOnClickListener {
            finish()
        }
    }

    override fun onOptionsItemSelected(item: android.view.MenuItem): Boolean {
        if (item.itemId == android.R.id.home) {
            finish() // 点返回箭头，关掉本页
            return true
        }
        return super.onOptionsItemSelected(item)
    }
}
