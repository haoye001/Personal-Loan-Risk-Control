package com.example.loanapp

import android.os.Bundle
import android.widget.Button
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import org.json.JSONArray

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
                MyLoan(products[loanId]?.name ?: "产品 #$loanId", application.getString("amount"),
                    applicationStatus(application.getString("status")))
            }
        }) { loans ->
            list.adapter = MyLoanAdapter(loans)
            if (loans.isEmpty()) showMessage("暂无贷款申请")
        }
    }
}
