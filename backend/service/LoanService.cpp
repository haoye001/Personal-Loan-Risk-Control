#include "LoanService.h"
#include "../utils/Logger.h"

bool LoanService::getAllLoans(std::vector<Loan>& loans)
{
    return loanDAO.getAllLoans(loans);
}

bool LoanService::getLoanById(int id, Loan& loan)
{
    if (id <= 0)
    {
        LOG_WARN("LoanService: invalid loan id = " + std::to_string(id));
        return false;
    }

    return loanDAO.getLoanById(id, loan);
}

bool LoanService::createLoan(const Loan& loan)
{
    if (loan.name.empty() || loan.maxAmount <= 0 || loan.term <= 0)
    {
        LOG_WARN("LoanService: invalid loan parameters");
        return false;
    }

    return loanDAO.createLoan(loan);
}

bool LoanService::updateLoan(const Loan& loan)
{
    if (loan.id <= 0 || loan.name.empty()
        || loan.maxAmount <= 0 || loan.term <= 0)
    {
        LOG_WARN("LoanService: invalid loan update parameters");
        return false;
    }

    return loanDAO.updateLoan(loan);
}
