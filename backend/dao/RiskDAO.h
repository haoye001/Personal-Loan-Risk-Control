#pragma once
#include "../model/RiskResult.h"
class RiskDAO{
    public:
    bool saveRiskResult(const RiskResult&result);
    bool getRiskResultByApplicationId(
        int applicationId,
        RiskResult& result
    );
};