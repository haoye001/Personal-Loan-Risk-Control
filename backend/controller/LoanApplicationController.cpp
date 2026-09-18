#include "LoanApplicationController.h"
#include <iostream>
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
        for(auto& app : applications)
        {
            Json::Value item;
            item["id"] = app.id;
            item["loanId"] = app.loanId;
            item["amount"] = app.amount;
            item["status"] = app.status;
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