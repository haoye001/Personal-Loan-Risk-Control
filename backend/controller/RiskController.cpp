#include "RiskController.h"
void RiskController::checkRisk(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&
    )>&& callback
)
{
    auto json = req->getJsonObject();
    Json::Value responseJson;
    if (!json)
    {
        responseJson["success"] = false;
        responseJson["message"] = "invalid json";
        callback(
            drogon::HttpResponse::newHttpJsonResponse(
                responseJson
            )
        );
        return;
    }
    // 获取请求参数
    int userId = (*json)["userId"].asInt();
    int applicationId = (*json)["applicationId"].asInt();
    LoanApplication application;
    application.id = applicationId;
    application.userId = userId;
    application.loanId = (*json)["loanId"].asInt();
    application.amount = (*json)["amount"].asDouble();
    // 查询用户
    User user;
    if (!userDAO.getUserById(userId, user))
    {
        responseJson["success"] = false;
        responseJson["message"] = "user not found";
        callback(
            drogon::HttpResponse::newHttpJsonResponse(
                responseJson
            )
        );
        return;
    }
    // 计算风险
    RiskResult riskResult =
        riskService.calculateRisk(
            user,
            application
        );
    // 设置申请 ID
    riskResult.applicationId = applicationId;
    // 保存风险结果
    if (!riskDAO.saveRiskResult(riskResult))
    {
        responseJson["success"] = false;
        responseJson["message"] = "save risk result failed";
        callback(
            drogon::HttpResponse::newHttpJsonResponse(
                responseJson
            )
        );
        return;
    }
    // 返回结果
    responseJson["success"] = true;
    responseJson["applicationId"] = riskResult.applicationId;
    responseJson["score"] = riskResult.score;
    responseJson["level"] = riskResult.level;
    responseJson["decision"] = riskResult.decision;
    callback(
        drogon::HttpResponse::newHttpJsonResponse(
            responseJson
        )
    );
}
void RiskController::getRiskResult(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&
    )>&& callback,
    int applicationId
)
{
    Json::Value responseJson;

    RiskResult result;

    if (!riskDAO.getRiskResultByApplicationId(
            applicationId,
            result))
    {
        responseJson["success"] = false;
        responseJson["message"] = "risk result not found";
        auto response=
            drogon::HttpResponse::newHttpJsonResponse(
                responseJson
            );
        response->setStatusCode(
            drogon::k404NotFound
        );
        callback(response);
        return;
    }

    responseJson["success"] = true;
    responseJson["applicationId"] = result.applicationId;
    responseJson["score"] = result.score;
    responseJson["level"] = result.level;
    responseJson["decision"] = result.decision;

    callback(
        drogon::HttpResponse::newHttpJsonResponse(
            responseJson
        )
    );
}