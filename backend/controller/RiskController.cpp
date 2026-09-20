#include "RiskController.h"
#include "../dao/LoanApplicationDAO.h"
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
    // 检查必要参数是否存在
if (!json->isMember("userId") ||
    !json->isMember("applicationId") ||
    !json->isMember("loanId") ||
    !json->isMember("amount"))
{
    responseJson["success"] = false;
    responseJson["message"] = "missing required parameters";
    auto response =
        drogon::HttpResponse::newHttpJsonResponse(
            responseJson
        );
    response->setStatusCode(
        drogon::k400BadRequest
    );
    callback(response);
    return;
}
    // 获取请求参数
// 获取请求参数
int userId = (*json)["userId"].asInt();
int applicationId = (*json)["applicationId"].asInt();
int loanId = (*json)["loanId"].asInt();
double amount = (*json)["amount"].asDouble();

// 检查参数是否合法
if (userId <= 0 ||
    applicationId <= 0 ||
    loanId <= 0 ||
    amount <= 0)
{
    responseJson["success"] = false;
    responseJson["message"] = "invalid parameter values";

    auto response =
        drogon::HttpResponse::newHttpJsonResponse(
            responseJson
        );

    response->setStatusCode(
        drogon::k400BadRequest
    );

    callback(response);
    return;
}
    LoanApplication application;
    application.id = applicationId;
    application.userId = userId;
    application.loanId = (*json)["loanId"].asInt();
    application.amount = (*json)["amount"].asDouble();
    // 检查申请是否存在，以及是否属于当前用户
LoanApplicationDAO applicationDAO;
LoanApplication dbApplication;

if (!applicationDAO.getApplicationById(
        applicationId,
        dbApplication))
{
    responseJson["success"] = false;
    responseJson["message"] = "application not found";

    auto response =
        drogon::HttpResponse::newHttpJsonResponse(
            responseJson
        );

    response->setStatusCode(
        drogon::k404NotFound
    );

    callback(response);
    return;
}
if (dbApplication.userId != userId)
{
    responseJson["success"] = false;
    responseJson["message"] = "application does not belong to user";
    auto response =
        drogon::HttpResponse::newHttpJsonResponse(
            responseJson
        );
    response->setStatusCode(
        drogon::k403Forbidden
    );

    callback(response);
    return;
}
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
// 根据风险决策更新申请状态
std::string applicationStatus;
if (riskResult.decision == "approved")
{
    applicationStatus = "approved";
}
else if (riskResult.decision == "rejected")
{
    applicationStatus = "rejected";
}
else if (riskResult.decision == "manual_review")
{
    applicationStatus = "manual_review";
}
else
{
    applicationStatus = "pending";
}
// 更新贷款申请状态


if (!applicationDAO.updateApplicationStatus(
        applicationId,
        applicationStatus))
{
    responseJson["success"] = false;
    responseJson["message"] =
        "risk saved, but update application status failed";

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
