#pragma once

#include "../model/LoanApplication.h"
#include <vector>
#include <string>

class LoanApplicationDAO
{
public:
    bool insertApplication(
        const LoanApplication& application,
        int& newId
    );

    bool getApplicationsByUserId(
        int userId,
        std::vector<LoanApplication>& applications
    );

    bool getApplicationById(
        int applicationId,
        LoanApplication& application
    );

    bool updateApplicationStatus(
        int applicationId,
        const std::string& status
    );

    // 检查用户是否已存在非终态的申请
    bool hasPendingApplication(
        int userId,
        int loanId
    );

    // 安全的状态更新：先校验当前状态
    bool transitionApplicationStatus(
        int applicationId,
        const std::string& fromStatus,
        const std::string& toStatus
    );
};
