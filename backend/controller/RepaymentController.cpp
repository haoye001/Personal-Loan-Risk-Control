#include "RepaymentController.h"

Json::Value RepaymentController::toJson(
    const std::vector<Repayment>& repayments)
{
    Json::Value list(Json::arrayValue);
    for (const Repayment& repayment : repayments)
    {
        Json::Value item;
        item["id"] = repayment.id;
        item["applicationId"] = repayment.applicationId;
        item["installmentNumber"] = repayment.installmentNumber;
        item["principalAmount"] = repayment.principalAmount;
        item["interestAmount"] = repayment.interestAmount;
        item["dueAmount"] = repayment.dueAmount;
        item["paidAmount"] = repayment.paidAmount;
        item["remainingAmount"] = repayment.remainingAmount;
        item["dueDate"] = repayment.dueDate;
        item["status"] = repayment.status;
        list.append(item);
    }
    return list;
}

drogon::HttpStatusCode RepaymentController::statusForError(
    const std::string& error)
{
    if (error.compare(0, 15, "database error:") == 0)
    {
        return drogon::k500InternalServerError;
    }
    if (error == "application not found" || error == "repayment plan not found")
    {
        return drogon::k404NotFound;
    }
    if (error == "only approved applications can be disbursed"
        || error == "repayment plan already exists"
        || error == "application is not eligible for repayment"
        || error == "payment exceeds outstanding balance"
        || error == "application status changed during disbursement"
        || error == "application status changed during repayment")
    {
        return drogon::k409Conflict;
    }
    return drogon::k400BadRequest;
}

void RepaymentController::getRepayments(
    const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int applicationId)
{
    std::vector<Repayment> repayments;
    std::string error;
    if (!repaymentService.getRepayments(applicationId, repayments, error))
    {
        LOG_WARN("RepaymentController: query failed, " + error);
        callback(ApiResponse::failureResponse(error, statusForError(error)));
        return;
    }

    if (repayments.empty())
    {
        callback(ApiResponse::failureResponse(
            "repayment plan not found", drogon::k404NotFound));
        return;
    }

    Json::Value data;
    data["applicationId"] = applicationId;
    data["repayments"] = toJson(repayments);
    callback(ApiResponse::successResponse("success", data));
}

void RepaymentController::makePayment(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    const auto json = req->getJsonObject();
    if (!json
        || !json->isMember("applicationId")
        || !(*json)["applicationId"].isInt()
        || !json->isMember("amount")
        || !(*json)["amount"].isNumeric())
    {
        callback(ApiResponse::failureResponse(
            "applicationId and numeric amount are required",
            drogon::k400BadRequest));
        return;
    }

    const int applicationId = (*json)["applicationId"].asInt();
    const double amount = (*json)["amount"].asDouble();
    std::vector<Repayment> repayments;
    bool completed = false;
    std::string error;

    if (!repaymentService.makePayment(
            applicationId, amount, repayments, completed, error))
    {
        LOG_WARN("RepaymentController: payment failed, " + error);
        callback(ApiResponse::failureResponse(error, statusForError(error)));
        return;
    }

    Json::Value data;
    data["applicationId"] = applicationId;
    data["status"] = completed ? "completed" : "repaying";
    data["repayments"] = toJson(repayments);
    callback(ApiResponse::successResponse("payment success", data));
}
