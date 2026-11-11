#pragma once

#include "../model/LoanApplication.h"
#include "../dao/LoanApplicationDAO.h"
#include "../dao/UserDAO.h"
#include "../dao/LoanDAO.h"
#include <vector>
#include <string>

class LoanApplicationService
{
private:
    LoanApplicationDAO loanApplicationDAO;
    UserDAO userDAO;
    LoanDAO loanDAO;

public:
    // 提交申请
    // 返回值：true 成功，outNewId 为新建申请 ID
    bool applyLoan(
        const LoanApplication& application,
        int& outNewId,
        std::string& outError
    );

    bool getApplicationsByUserId(
        int userId,
        std::vector<LoanApplication>& applications
    );

    bool getApplicationById(
        int applicationId,
        LoanApplication& application
    );

    // 审批申请
    bool reviewApplication(
        int applicationId,
        const std::string& decision,
        std::string& outError
    );
};
