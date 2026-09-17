#pragma once
#include "../model/Loan.h"
#include "../service/LoanService.h"
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

    METHOD_LIST_END

    void getAllLoans(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& callback
    );
};