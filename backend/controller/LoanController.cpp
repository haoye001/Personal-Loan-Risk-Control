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

    if (!loanService.getAllLoans(loans))
    {
        LOG_ERROR("LoanController: getAllLoans failed");
        callback(ApiResponse::failureResponse(
            "failed to get loan products",
            drogon::k500InternalServerError));
        return;
    }

    Json::Value data(Json::arrayValue);

    for (const auto& loan : loans)
    {
        Json::Value item;
        item["id"] = loan.id;
        item["name"] = loan.name;
        item["maxAmount"] = loan.maxAmount;
        item["term"] = loan.term;
        data.append(item);
    }

    LOG_INFO("LoanController: getAllLoans, count = "
             + std::to_string(loans.size()));

    callback(ApiResponse::successResponse("success", data));
}

void LoanController::getLoanById(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&
    )>&& callback,
    int id
)
{
    Loan loan;

    if (!loanService.getLoanById(id, loan))
    {
        LOG_WARN("LoanController: loan not found, id = " + std::to_string(id));
        callback(ApiResponse::failureResponse(
            "loan not found",
            drogon::k404NotFound));
        return;
    }

    Json::Value data;
    data["id"] = loan.id;
    data["name"] = loan.name;
    data["maxAmount"] = loan.maxAmount;
    data["term"] = loan.term;

    callback(ApiResponse::successResponse("success", data));
}

void LoanController::createLoan(
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

    if (!json->isMember("name")
        || !json->isMember("maxAmount")
        || !json->isMember("term"))
    {
        callback(ApiResponse::failureResponse(
            "missing required parameters",
            drogon::k400BadRequest));
        return;
    }

    Loan loan;
    loan.name = (*json)["name"].asString();
    loan.maxAmount = (*json)["maxAmount"].asDouble();
    loan.term = (*json)["term"].asInt();

    if (loan.name.empty()
        || loan.maxAmount <= 0
        || loan.term <= 0)
    {
        callback(ApiResponse::failureResponse(
            "invalid loan parameters",
            drogon::k400BadRequest));
        return;
    }

    if (!loanService.createLoan(loan))
    {
        LOG_ERROR("LoanController: createLoan failed");
        callback(ApiResponse::failureResponse(
            "create loan failed",
            drogon::k500InternalServerError));
        return;
    }

    LOG_INFO("LoanController: createLoan name = " + loan.name);
    callback(ApiResponse::successResponse("loan created"));
}

void LoanController::updateLoan(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&
    )>&& callback,
    int id
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

    if (!json->isMember("name")
        || !json->isMember("maxAmount")
        || !json->isMember("term"))
    {
        callback(ApiResponse::failureResponse(
            "missing required parameters",
            drogon::k400BadRequest));
        return;
    }

    Loan loan;
    loan.id = id;
    loan.name = (*json)["name"].asString();
    loan.maxAmount = (*json)["maxAmount"].asDouble();
    loan.term = (*json)["term"].asInt();

    if (id <= 0
        || loan.name.empty()
        || loan.maxAmount <= 0
        || loan.term <= 0)
    {
        callback(ApiResponse::failureResponse(
            "invalid loan parameters",
            drogon::k400BadRequest));
        return;
    }

    if (!loanService.updateLoan(loan))
    {
        LOG_WARN("LoanController: updateLoan failed, id = " + std::to_string(id));
        callback(ApiResponse::failureResponse(
            "update loan failed",
            drogon::k400BadRequest));
        return;
    }

    LOG_INFO("LoanController: updateLoan id = " + std::to_string(id));
    callback(ApiResponse::successResponse("loan updated"));
}
