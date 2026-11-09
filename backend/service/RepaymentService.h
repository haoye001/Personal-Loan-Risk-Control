#pragma once

#include "../dao/RepaymentDAO.h"
#include "../model/Repayment.h"
#include <string>
#include <vector>

class RepaymentService
{
private:
    RepaymentDAO repaymentDAO;

public:
    bool disburseApplication(
        int applicationId,
        std::string& outError
    );

    bool getRepayments(
        int applicationId,
        std::vector<Repayment>& repayments,
        std::string& outError
    );

    bool makePayment(
        int applicationId,
        double amount,
        std::vector<Repayment>& repayments,
        bool& completed,
        std::string& outError
    );
};
