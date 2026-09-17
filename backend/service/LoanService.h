#pragma once
#include <vector>
#include "../model/Loan.h"
#include "../dao/LoanDAO.h"
class LoanService
{
private:
    LoanDAO loanDAO;
public:
    bool getAllLoans(std::vector<Loan>& loans);
};