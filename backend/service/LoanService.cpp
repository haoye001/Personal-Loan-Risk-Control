#include "LoanService.h"
bool LoanService::getAllLoans(std::vector<Loan>& loans)
{
    return loanDAO.getAllLoans(loans);
}