package com.example.loanapp

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView

class MyLoanAdapter(private val loans: List<MyLoan>) : RecyclerView.Adapter<MyLoanAdapter.LoanViewHolder>() {

    class LoanViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        val textProductName: TextView = itemView.findViewById(R.id.textProductName)
        val textStatus: TextView = itemView.findViewById(R.id.textStatus)
        val textDetail: TextView = itemView.findViewById(R.id.textDetail)
    }

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): LoanViewHolder {
        val view = LayoutInflater.from(parent.context)
            .inflate(R.layout.item_my_loan, parent, false)
        return LoanViewHolder(view)
    }

    override fun onBindViewHolder(holder: LoanViewHolder, position: Int) {
        val loan = loans[position]
        holder.textProductName.text = loan.productName
        holder.textStatus.text = loan.status
        holder.textDetail.text = "金额 ${loan.amount} 元，期限 ${loan.months} 个月"
    }

    override fun getItemCount(): Int = loans.size
}
