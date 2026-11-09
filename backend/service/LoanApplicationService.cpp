#include "LoanApplicationService.h"
#include "../model/User.h"
#include "../model/Loan.h"
#include "../utils/Logger.h"
#include "../utils/StateMachine.h"

bool LoanApplicationService::applyLoan(
    const LoanApplication& application,
    int& outNewId,
    std::string& outError)
{
    outNewId = 0;

    // 参数校验
    if (application.userId <= 0)
    {
        outError = "invalid userId";
        LOG_WARN("LoanApplicationService: " + outError);
        return false;
    }

    if (application.loanId <= 0)
    {
        outError = "invalid loanId";
        LOG_WARN("LoanApplicationService: " + outError);
        return false;
    }

    if (application.amount <= 0)
    {
        outError = "invalid amount";
        LOG_WARN("LoanApplicationService: " + outError);
        return false;
    }

    // 用户是否存在
    User user;
    if (!userDAO.getUserById(application.userId, user))
    {
        outError = "user not found";
        LOG_WARN("LoanApplicationService: user not found, id = "
                 + std::to_string(application.userId));
        return false;
    }

    // 产品是否存在
    Loan loan;
    if (!loanDAO.getLoanById(application.loanId, loan))
    {
        outError = "loan product not found";
        LOG_WARN("LoanApplicationService: loan not found, id = "
                 + std::to_string(application.loanId));
        return false;
    }

    // 金额是否超过产品上限
    if (application.amount > loan.maxAmount)
    {
        outError = "amount exceeds loan max amount";
        LOG_WARN("LoanApplicationService: amount exceeds max, amount = "
                 + std::to_string(application.amount)
                 + ", max = " + std::to_string(loan.maxAmount));
        return false;
    }

    // 防止重复申请
    if (loanApplicationDAO.hasPendingApplication(
            application.userId,
            application.loanId))
    {
        outError = "duplicate application";
        LOG_WARN("LoanApplicationService: duplicate application");
        return false;
    }

    return loanApplicationDAO.insertApplication(application, outNewId);
}

bool LoanApplicationService::getApplicationsByUserId(
    int userId,
    std::vector<LoanApplication>& applications)
{
    if (userId <= 0)
    {
        LOG_WARN("LoanApplicationService: invalid userId = "
                 + std::to_string(userId));
        return false;
    }

    return loanApplicationDAO.getApplicationsByUserId(userId, applications);
}

bool LoanApplicationService::getApplicationById(
    int applicationId,
    LoanApplication& application)
{
    if (applicationId <= 0)
    {
        return false;
    }

    return loanApplicationDAO.getApplicationById(applicationId, application);
}

bool LoanApplicationService::reviewApplication(
    int applicationId,
    const std::string& decision,
    std::string& outError)
{
    if (applicationId <= 0)
    {
        outError = "invalid applicationId";
        return false;
    }

    if (decision != "approved" && decision != "rejected")
    {
        outError = "invalid decision";
        return false;
    }

    // 查询申请
    LoanApplication application;
    if (!loanApplicationDAO.getApplicationById(applicationId, application))
    {
        outError = "application not found";
        return false;
    }

    // 状态机检查
    if (!ApplicationStateMachine::canTransition(application.status, decision))
    {
        outError = "invalid status transition from " + application.status
                   + " to " + decision;
        LOG_WARN("LoanApplicationService: " + outError);
        return false;
    }

    return loanApplicationDAO.transitionApplicationStatus(
        applicationId,
        application.status,
        decision);
}
