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
        val textRisk: TextView = itemView.findViewById(R.id.textRisk)
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
        holder.textDetail.text = "金额 ${loan.amount} 元"

        val levelText = riskLevelText(loan.level)
        if (loan.score != null) {
            holder.textRisk.visibility = View.VISIBLE
            holder.textRisk.text = if (levelText.isEmpty()) {
                "风控评分 ${loan.score}"
            } else {
                "风控评分 ${loan.score} · $levelText"
            }
        } else {
            holder.textRisk.visibility = View.GONE
        }
    }

    override fun getItemCount(): Int = loans.size
}
