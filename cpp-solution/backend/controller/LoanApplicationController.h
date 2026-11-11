#pragma once

#include "../service/LoanApplicationService.h"
#include "../service/RepaymentService.h"
#include "../utils/ApiResponse.h"
#include "../utils/Logger.h"
#include <drogon/drogon.h>

class LoanApplicationController
    : public drogon::HttpController<LoanApplicationController>
{
private:
    LoanApplicationService loanApplicationService;
    RepaymentService repaymentService;

public:
    METHOD_LIST_BEGIN

    ADD_METHOD_TO(
        LoanApplicationController::applyLoan,
        "/api/apply",
        drogon::Post
    );

    ADD_METHOD_TO(
        LoanApplicationController::getApplications,
        "/api/applications/{1}",
        drogon::Get
    );

    ADD_METHOD_TO(
        LoanApplicationController::getApplicationById,
        "/api/applications/detail/{1}",
        drogon::Get
    );

    ADD_METHOD_TO(
        LoanApplicationController::reviewApplication,
        "/api/applications/review",
        drogon::Put
    );

    ADD_METHOD_TO(
        LoanApplicationController::disburseApplication,
        "/api/applications/{1}/disburse",
        drogon::Post
    );

    METHOD_LIST_END

    void applyLoan(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& callback
    );

    void getApplications(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& callback,
        int userId
    );

    void getApplicationById(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& callback,
        int applicationId
    );

    void reviewApplication(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& callback
    );

    void disburseApplication(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& callback,
        int applicationId
    );
};
