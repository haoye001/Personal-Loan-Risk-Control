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
    Json::Value result;
    if(!json)
    {
        result["success"] = false;
        result["message"] = "invalid json";
        callback(
            drogon::HttpResponse::newHttpJsonResponse(result)
        );
        return;
    }
    LoanApplication application;
    application.userId =
        (*json)["userId"].asInt();
    application.loanId =
        (*json)["loanId"].asInt();
    application.amount =
        (*json)["amount"].asDouble();
    if(loanApplicationService.applyLoan(application))
    {
        result["success"] = true;
        result["message"] = "apply success";
    }
    else
    {
        result["success"] = false;
        result["message"] = "apply failed";
    }
    callback(
        drogon::HttpResponse::newHttpJsonResponse(result)
    );
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
    Json::Value result;
    if(loanApplicationService
        .getApplicationsByUserId(
            userId,
            applications))
    {
        result["success"] = true;
        Json::Value list(Json::arrayValue);
        for(const auto& app : applications)
        {
            Json::Value item;
            item["id"] = app.id;
            item["loanId"] = app.loanId;
            item["amount"] = app.amount;
            item["status"] = app.status;
            //风险评估结果
            item["riskScore"]=app.riskScore;
            item["riskLevel"]=app.riskLevel;
            item["decision"]=app.decision;
            list.append(item);
        }
        result["applications"] = list;
    }
    else
    {
        result["success"] = false;
        result["message"] = "query failed";
    }
    callback(
        drogon::HttpResponse::newHttpJsonResponse(result)
    );
}
void LoanApplicationController::reviewApplication(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
)
{
    Json::Value responseJson;

    auto json = req->getJsonObject();

    if (!json ||
        !json->isMember("applicationId") ||
        !json->isMember("decision"))
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

    int applicationId =
        (*json)["applicationId"].asInt();

    std::string decision =
        (*json)["decision"].asString();

    if (applicationId <= 0 ||
        (decision != "approved" &&
         decision != "rejected"))
    {
        responseJson["success"] = false;
        responseJson["message"] =
            "invalid applicationId or decision";

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

    LoanApplicationDAO dao;

    bool success = dao.updateApplicationStatus(
        applicationId,
        decision
    );
    if (success)
{
    RiskDAO riskDAO;

    success = riskDAO.updateRiskDecision(
        applicationId,
        decision
    );
}
    if (!success)
    {
        responseJson["success"] = false;
        responseJson["message"] =
            "failed to update application status";

        auto response =
            drogon::HttpResponse::newHttpJsonResponse(
                responseJson
            );

        response->setStatusCode(
            drogon::k500InternalServerError
        );

        callback(response);
        return;
    }

    responseJson["success"] = true;
    responseJson["applicationId"] = applicationId;
    responseJson["status"] = decision;

    auto response =
        drogon::HttpResponse::newHttpJsonResponse(
            responseJson
        );

    callback(response);
}