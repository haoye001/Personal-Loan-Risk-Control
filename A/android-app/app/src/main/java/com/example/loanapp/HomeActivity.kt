package com.example.loanapp

import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.widget.TextView
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import okhttp3.OkHttpClient
import okhttp3.Request

class HomeActivity : AppCompatActivity() {

    private lateinit var adapter: LoanAdapter

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_home)

        val phone = intent.getStringExtra("phone")
        findViewById<TextView>(R.id.textViewWelcome).text = "欢迎你，$phone"

        // 我的贷款按钮
        findViewById<android.widget.Button>(R.id.buttonMyLoans).setOnClickListener {
            startActivity(Intent(this, MyLoansActivity::class.java))
        }

        // 个人中心按钮
        findViewById<android.widget.Button>(R.id.buttonProfile).setOnClickListener {
            val intent = Intent(this, ProfileActivity::class.java)
            intent.putExtra("phone", phone)
            startActivity(intent)
        }

        // 写死 5 条假贷款产品数据
        val fakeLoans = listOf(
            LoanProduct("极速贷", 50000, 0.03),
            LoanProduct("工薪贷", 200000, 0.02),
            LoanProduct("车主贷", 300000, 0.025),
            LoanProduct("房抵贷", 1000000, 0.015),
            LoanProduct("经营贷", 500000, 0.028)
        )

        adapter = LoanAdapter(fakeLoans) { product ->
            // 点了某一行，跳转到申请页，带上产品信息
            val intent = Intent(this, ApplyActivity::class.java)
            intent.putExtra("productName", product.name)
            intent.putExtra("maxAmount", product.maxAmount)
            intent.putExtra("rate", product.rate)
            startActivity(intent)
        }

        val recyclerView = findViewById<RecyclerView>(R.id.recyclerViewLoans)
        recyclerView.layoutManager = LinearLayoutManager(this)
        recyclerView.adapter = adapter

        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }
    }
}
