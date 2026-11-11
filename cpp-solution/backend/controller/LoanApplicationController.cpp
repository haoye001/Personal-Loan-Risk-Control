#include "LoanApplicationController.h"

#include <iostream>
#include "../dao/RiskDAO.h"

void LoanApplicationController::applyLoan(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&
    )>&& callback
)
{
    auto json = req->getJsonObject();

    if (!json)
    {
        callback(ApiResponse::failureResponse(
            "invalid json",
            drogon::k400BadRequest));
        return;
    }

    if (!json->isMember("userId")
        || !json->isMember("loanId")
        || !json->isMember("amount"))
    {
        callback(ApiResponse::failureResponse(
            "missing required parameters",
            drogon::k400BadRequest));
        return;
    }

    LoanApplication application;
    application.userId = (*json)["userId"].asInt();
    application.loanId = (*json)["loanId"].asInt();
    application.amount = (*json)["amount"].asDouble();

    int newId = 0;
    std::string error;

    if (!loanApplicationService.applyLoan(application, newId, error))
    {
        LOG_WARN("LoanApplicationController: apply failed, " + error);
        callback(ApiResponse::failureResponse(error, drogon::k400BadRequest));
        return;
    }

    Json::Value data;
    data["applicationId"] = newId;

    LOG_INFO("LoanApplicationController: apply success, id = "
             + std::to_string(newId));

    callback(ApiResponse::successResponse("apply success", data));
}

void LoanApplicationController::getApplications(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&
    )>&& callback,
    int userId
)
{
    std::vector<LoanApplication> applications;

    if (!loanApplicationService.getApplicationsByUserId(userId, applications))
    {
        callback(ApiResponse::failureResponse("query failed"));
        return;
    }

    Json::Value list(Json::arrayValue);

    for (const auto& app : applications)
    {
        Json::Value item;
        item["id"] = app.id;
        item["loanId"] = app.loanId;
        item["amount"] = app.amount;
        item["status"] = app.status;
        item["createdAt"] = app.createdAt;
        item["riskScore"] = app.riskScore;
        item["riskLevel"] = app.riskLevel;
        item["decision"] = app.decision;
        list.append(item);
    }

    Json::Value data;
    data["applications"] = list;

    callback(ApiResponse::successResponse("success", data));
}

void LoanApplicationController::getApplicationById(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&
    )>&& callback,
    int applicationId
)
{
    LoanApplication application;

    if (!loanApplicationService.getApplicationById(applicationId, application))
    {
        callback(ApiResponse::failureResponse(
            "application not found",
            drogon::k404NotFound));
        return;
    }

    Json::Value data;
    data["id"] = application.id;
    data["userId"] = application.userId;
    data["loanId"] = application.loanId;
    data["amount"] = application.amount;
    data["status"] = application.status;
    data["createdAt"] = application.createdAt;

    callback(ApiResponse::successResponse("success", data));
}

void LoanApplicationController::reviewApplication(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
)
{
    auto json = req->getJsonObject();

    if (!json
        || !json->isMember("applicationId")
        || !json->isMember("decision"))
    {
        callback(ApiResponse::failureResponse(
            "missing required parameters",
            drogon::k400BadRequest));
        return;
    }

    int applicationId = (*json)["applicationId"].asInt();
    std::string decision = (*json)["decision"].asString();

    if (applicationId <= 0
        || (decision != "approved"
            && decision != "rejected"))
    {
        callback(ApiResponse::failureResponse(
            "invalid applicationId or decision",
            drogon::k400BadRequest));
        return;
    }

    std::string error;

    if (!loanApplicationService.reviewApplication(
            applicationId,
            decision,
            error))
    {
        LOG_WARN("LoanApplicationController: review failed, " + error);
        callback(ApiResponse::failureResponse(
            error,
            drogon::k400BadRequest));
        return;
    }

    // 同步更新最新风控记录决策
    RiskDAO riskDAO;
    riskDAO.updateRiskDecision(applicationId, decision);

    LOG_INFO("LoanApplicationController: review success, applicationId = "
             + std::to_string(applicationId)
             + ", decision = " + decision);

    Json::Value data;
    data["applicationId"] = applicationId;
    data["status"] = decision;

    callback(ApiResponse::successResponse("review success", data));
}

void LoanApplicationController::disburseApplication(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&
    )>&& callback,
    int applicationId
)
{
    if (applicationId <= 0)
    {
        callback(ApiResponse::failureResponse(
            "invalid applicationId",
            drogon::k400BadRequest));
        return;
    }

    std::string error;
    if (!repaymentService.disburseApplication(applicationId, error))
    {
        LOG_WARN("LoanApplicationController: disbursement failed, " + error);
        drogon::HttpStatusCode status = drogon::k400BadRequest;
        if (error == "application not found")
        {
            status = drogon::k404NotFound;
        }
        else if (error.compare(0, 15, "database error:") == 0)
        {
            status = drogon::k500InternalServerError;
        }
        else if (error == "only approved applications can be disbursed"
                 || error == "repayment plan already exists"
                 || error == "application status changed during disbursement")
        {
            status = drogon::k409Conflict;
        }
        callback(ApiResponse::failureResponse(
            error,
            status));
        return;
    }

    LOG_INFO("LoanApplicationController: disburse success, applicationId = "
             + std::to_string(applicationId));

    Json::Value data;
    data["applicationId"] = applicationId;
    data["status"] = "disbursed";

    callback(ApiResponse::successResponse("disburse success", data));
}
