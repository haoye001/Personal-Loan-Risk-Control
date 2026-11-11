package com.example.loanapp

data class MyLoan(val productName: String, val amount: String, val status: String)

internal fun applicationStatus(value: String): String = when (value) {
    "pending" -> "等待风控"
    "manual_review" -> "待人工审核"
    "approved" -> "已通过"
    "rejected" -> "已拒绝"
    "disbursed" -> "已放款"
    "repaying" -> "还款中"
    "completed" -> "已结清"
    else -> value
}
