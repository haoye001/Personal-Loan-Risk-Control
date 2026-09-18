#pragma once
#include "../model/LoanApplication.h"
#include <vector>
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
};