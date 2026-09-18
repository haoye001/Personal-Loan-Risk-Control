#pragma once
#include <string>
struct LoanApplication
{
    int id;
    int userId;
    int loanId;
    double amount;
    std::string status;
    std::string createdAt;
};