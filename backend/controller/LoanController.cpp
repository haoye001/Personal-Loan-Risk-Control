#include "LoanController.h"

#include <iostream>
#include <json/json.h>

void LoanController::getAllLoans(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&
    )>&& callback
)
{
    std::vector<Loan> loans;

    bool success = loanService.getAllLoans(loans);

    if(!success)
    {
        Json::Value response;
        response["success"] = false;
        response["message"] = "failed to get loan products";

        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            response
        );

        resp->setStatusCode(
            drogon::k500InternalServerError
        );

        callback(resp);
        return;
    }

    Json::Value response;
    response["success"] = true;

    Json::Value loanList(Json::arrayValue);

    for(const auto& loan : loans)
    {
        Json::Value item;

        item["id"] = loan.id;
        item["name"] = loan.name;
        item["maxAmount"] = loan.maxAmount;
        item["term"] = loan.term;

        loanList.append(item);
    }

    response["loans"] = loanList;

    auto resp = drogon::HttpResponse::newHttpJsonResponse(
        response
    );

    callback(resp);
}