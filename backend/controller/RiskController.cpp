#include "RiskController.h"
void RiskController::checkRisk(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&
    )>&& callback
)
{
    auto json=req->getJsonObject();
    Json::Value result;
    if(!json)
    {
        result["success"]=false;
        result["message"]="invalid json";
        callback(
            drogon::HttpResponse::newHttpJsonResponse(result)
        );
        return;
    }
    int userId =
        (*json)["userId"].asInt();
    LoanApplication application;
    application.userId=userId;
    application.loanId =
        (*json)["loanId"].asInt();
    application.amount =
        (*json)["amount"].asDouble();
    User user;
    if(!userDAO.getUserById(userId,user))
    {
        result["success"]=false;
        result["message"]="user not found";
        callback(
            drogon::HttpResponse::newHttpJsonResponse(result)
        );
        return;
    }
    RiskResult risk =
        riskService.calculateRisk(
            user,
            application
        );
    result["success"]=true;
    result["score"]=risk.score;
    result["level"]=risk.level;
    result["decision"]=risk.decision;
    callback(
        drogon::HttpResponse::newHttpJsonResponse(result)
    );
}