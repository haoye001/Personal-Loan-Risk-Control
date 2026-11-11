#pragma once

#include "../model/Loan.h"
#include "../service/LoanService.h"
#include "../utils/ApiResponse.h"
#include "../utils/Logger.h"
#include <drogon/drogon.h>
#include <functional>
#include <vector>

class LoanController
    : public drogon::HttpController<LoanController>
{
private:
    LoanService loanService;

public:
    METHOD_LIST_BEGIN

    ADD_METHOD_TO(
        LoanController::getAllLoans,
        "/api/loans",
        drogon::Get
    );

    ADD_METHOD_TO(
        LoanController::getLoanById,
        "/api/loans/{1}",
        drogon::Get
    );

    ADD_METHOD_TO(
        LoanController::createLoan,
        "/api/loans",
        drogon::Post
    );

    ADD_METHOD_TO(
        LoanController::updateLoan,
        "/api/loans/{1}",
        drogon::Put
    );

    METHOD_LIST_END

    void getAllLoans(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& callback
    );

    void getLoanById(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& callback,
        int id
    );

    void createLoan(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& callback
    );

    void updateLoan(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& callback,
        int id
    );
};
