#pragma once
#include <vector>
#include "../model/Loan.h"

class LoanDAO
{
public:
    bool getAllLoans(std::vector<Loan>& loans);
    bool getLoanById(int id, Loan& loan);
    bool createLoan(const Loan& loan);
    bool updateLoan(const Loan& loan);
};