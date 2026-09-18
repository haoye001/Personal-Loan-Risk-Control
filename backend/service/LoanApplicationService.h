#pragma once
#include "../model/LoanApplication.h"
#include "../dao/LoanApplicationDAO.h"
#include <vector>
class LoanApplicationService
{
private:
    LoanApplicationDAO loanApplicationDAO;
public:
    bool applyLoan(
        const LoanApplication& application
    );
    bool getApplicationsByUserId(
        int userId,
        std::vector<LoanApplication>& applications
    );
};