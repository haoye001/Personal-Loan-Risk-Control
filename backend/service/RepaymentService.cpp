#include "RepaymentService.h"

#include "../utils/Logger.h"
#include <cmath>
#include <limits>

bool RepaymentService::disburseApplication(
    int applicationId,
    std::string& outError)
{
    if (applicationId <= 0)
    {
        outError = "invalid applicationId";
        return false;
    }
    return repaymentDAO.createPlanAndDisburse(applicationId, outError);
}

bool RepaymentService::getRepayments(
    int applicationId,
    std::vector<Repayment>& repayments,
    std::string& outError)
{
    if (applicationId <= 0)
    {
        outError = "invalid applicationId";
        return false;
    }
    return repaymentDAO.getRepaymentsByApplicationId(
        applicationId, repayments, outError);
}

bool RepaymentService::makePayment(
    int applicationId,
    double amount,
    std::vector<Repayment>& repayments,
    bool& completed,
    std::string& outError)
{
    if (applicationId <= 0)
    {
        outError = "invalid applicationId";
        return false;
    }

    if (!std::isfinite(amount) || amount <= 0.0
        || amount * 100.0
            >= static_cast<double>(std::numeric_limits<long long>::max()))
    {
        outError = "invalid payment amount";
        return false;
    }

    const double amountInCents = amount * 100.0;
    const long long amountCents = std::llround(amountInCents);
    if (std::abs(amountInCents - amountCents) > 0.000001)
    {
        outError = "payment amount must have at most two decimal places";
        return false;
    }

    return repaymentDAO.makePayment(
        applicationId, amountCents, repayments, completed, outError);
}
