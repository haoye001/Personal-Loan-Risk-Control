#pragma once
#include <vector>
#include "../model/Loan.h"
class LoanDAO{
    public:
    bool getAllLoans(std::vector<Loan>& loans);
};