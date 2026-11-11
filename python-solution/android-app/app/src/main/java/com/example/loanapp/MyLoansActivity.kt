package com.example.loanapp

import android.os.Bundle
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import org.json.JSONArray
import org.json.JSONObject

class MyLoansActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_my_loans)
        val list = findViewById<RecyclerView>(R.id.recyclerViewMyLoans)
        list.layoutManager = LinearLayoutManager(this)
        findViewById<Button>(R.id.buttonBack).setOnClickListener { finish() }

        runRequest(action = {
            val products = LoanApi.products().associateBy { it.id }
            val applications = JSONArray(LoanApi.request("/applications/me"))
            List(applications.length()) { index ->
                val application = applications.getJSONObject(index)
                val loanId = application.getInt("loanId")
                val applicationId = application.getInt("applicationId")
                // 风控结果可能尚未生成（如仍在等待风控），取不到就留空
                val risk = runCatching {
                    val json = JSONObject(LoanApi.request("/applications/$applicationId/risk-result"))
                    json.getInt("score") to json.getString("level")
                }.getOrNull()
                MyLoan(
                    productName = products[loanId]?.name ?: "产品 #$loanId",
                    amount = application.getString("amount"),
                    status = applicationStatus(application.getString("status")),
                    score = risk?.first,
                    level = risk?.second,
                )
            }
        }) { loans ->
            list.adapter = MyLoanAdapter(loans)
            if (loans.isEmpty()) showMessage("暂无贷款申请")
        }
    }
}
