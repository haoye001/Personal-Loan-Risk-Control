#pragma once

#include "../service/RepaymentService.h"
#include "../utils/ApiResponse.h"
#include "../utils/Logger.h"
#include <drogon/drogon.h>
#include <functional>

class RepaymentController
    : public drogon::HttpController<RepaymentController>
{
private:
    RepaymentService repaymentService;

    static Json::Value toJson(const std::vector<Repayment>& repayments);
    static drogon::HttpStatusCode statusForError(const std::string& error);

public:
    METHOD_LIST_BEGIN

    ADD_METHOD_TO(
        RepaymentController::getRepayments,
        "/api/loans/{1}/repayments",
        drogon::Get
    );

    ADD_METHOD_TO(
        RepaymentController::makePayment,
        "/api/repayments",
        drogon::Post
    );

    METHOD_LIST_END

    void getRepayments(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int applicationId
    );

    void makePayment(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );
};
