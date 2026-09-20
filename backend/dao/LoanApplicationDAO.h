#pragma once
#include "../model/LoanApplication.h"
#include <vector>
#include <string>
class LoanApplicationDAO
{
public:
    bool insertApplication(
        const LoanApplication& application
    );
    bool getApplicationsByUserId(
        int userId,
        std::vector<LoanApplication>& applications
    );
    bool updateApplicationStatus(
    int applicationId,
    const std::string& status
    );
    bool getApplicationById(
    int applicationId,
    LoanApplication& application
    );
};