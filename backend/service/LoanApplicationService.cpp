#include "LoanApplicationService.h"
bool LoanApplicationService::applyLoan(
    const LoanApplication& application
){
    return loanApplicationDAO.insertApplication(
        application
    );
}
bool LoanApplicationService::getApplicationsByUserId(
    int userId,
    std::vector<LoanApplication>& applications
){
    return loanApplicationDAO.getApplicationsByUserId(
        userId,
        applications
    );

}