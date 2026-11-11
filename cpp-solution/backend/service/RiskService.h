#pragma once
#include "../model/User.h"
#include "../model/LoanApplication.h"
#include "../model/RiskResult.h"
class RiskService
{
public:
    RiskResult calculateRisk(
        const User& user,
        const LoanApplication& application
    );
};