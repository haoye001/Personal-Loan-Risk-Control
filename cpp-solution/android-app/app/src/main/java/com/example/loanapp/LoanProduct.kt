package com.example.loanapp

data class LoanProduct(
    val name: String,      // 产品名，如"极速贷"
    val maxAmount: Int,    // 最高额度，单位元
    val rate: Double       // 日利率，如 0.03
)
