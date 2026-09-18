#include "RiskService.h"
RiskResult RiskService::calculateRisk(
    const User& user,
    const LoanApplication& application
)
{
    RiskResult result;
    int score = 50;
    // 收入评分
    if(user.income >= 10000)
    {
        score += 30;
    }
    else if(user.income >= 5000)
    {
        score += 20;
    }
    else
    {
        score += 10;
    }
    // 负债扣分
    if(user.debt > user.income * 0.5)
    {
        score -= 20;
    }
    // 贷款金额风险
    if(application.amount > 50000)
    {
        score -= 20;
    }
    else if(application.amount > 10000)
    {
        score -= 10;
    }
    // 年龄因素
    if(user.age >= 22 && user.age <= 55)
    {
        score += 10;
    }
    result.score = score;
    if(score >= 80)
    {
        result.level = "LOW";
        result.decision = "approved";
    }
    else if(score >= 60)
    {
        result.level = "MEDIUM";
        result.decision = "manual_review";
    }
    else
    {
        result.level = "HIGH";
        result.decision = "rejected";
    }
    return result;
}