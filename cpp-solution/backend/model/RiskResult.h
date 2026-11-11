#pragma once
#include <string>
struct RiskResult
{   
    int applicationId;
    int score;              // 风险评分
    std::string level;      // LOW/MEDIUM/HIGH
    std::string decision;   // approved/rejected
};