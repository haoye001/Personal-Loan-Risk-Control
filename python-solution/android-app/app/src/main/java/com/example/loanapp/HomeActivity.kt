package com.example.loanapp

import android.content.Intent
import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import org.json.JSONObject

class HomeActivity : AppCompatActivity() {
    private var openingProfile = false
    private var loading = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_home)
        findViewById<Button>(R.id.buttonMyLoans).setOnClickListener {
            startActivity(Intent(this, MyLoansActivity::class.java))
        }
        findViewById<Button>(R.id.buttonProfile).setOnClickListener { openProfile() }
        findViewById<RecyclerView>(R.id.recyclerViewLoans).layoutManager = LinearLayoutManager(this)
    }

    override fun onResume() {
        super.onResume()
        openingProfile = false
        if (loading) return
        loading = true
        runRequest(action = {
            val profile = JSONObject(LoanApi.request("/users/me"))
            val complete = listOf("age", "income", "debt").none { profile.isNull(it) }
            profile.getString("username") to if (complete) LoanApi.products() else null
        }, onComplete = { loading = false }) { (name, products) ->
            findViewById<TextView>(R.id.textViewWelcome).text = if (products == null) {
                "请先在个人中心补全资料"
            } else if (products.isEmpty()) {
                "暂无可申请产品"
            } else {
                "欢迎你，$name"
            }
            findViewById<RecyclerView>(R.id.recyclerViewLoans).adapter = LoanAdapter(products.orEmpty()) { product ->
                startActivity(Intent(this, ApplyActivity::class.java).apply {
                    putExtra("loanId", product.id)
                    putExtra("productName", product.name)
                    putExtra("maxAmount", product.maxAmount)
                    putExtra("term", product.term)
                })
            }
        }
    }

    private fun openProfile() {
        if (openingProfile) return
        openingProfile = true
        startActivity(Intent(this, ProfileActivity::class.java))
    }
}
