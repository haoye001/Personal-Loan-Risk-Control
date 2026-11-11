#pragma once

#include <string>

struct Repayment
{
    int id;
    int applicationId;
    int installmentNumber;
    double principalAmount;
    double interestAmount;
    double dueAmount;
    double paidAmount;
    double remainingAmount;
    std::string dueDate;
    std::string status;
};
