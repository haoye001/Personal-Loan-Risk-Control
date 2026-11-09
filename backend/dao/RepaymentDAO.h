#pragma once

#include "../model/Repayment.h"
#include <string>
#include <vector>

class RepaymentDAO
{
public:
    bool createPlanAndDisburse(
        int applicationId,
        std::string& outError
    );

    bool getRepaymentsByApplicationId(
        int applicationId,
        std::vector<Repayment>& repayments,
        std::string& outError
    );

    bool makePayment(
        int applicationId,
        long long amountCents,
        std::vector<Repayment>& repayments,
        bool& completed,
        std::string& outError
    );
};
