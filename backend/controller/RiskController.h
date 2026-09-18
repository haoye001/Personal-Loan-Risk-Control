#pragma once
#include "../service/RiskService.h"
#include "../dao/UserDAO.h"
#include <drogon/drogon.h>
class RiskController
    : public drogon::HttpController<RiskController>
{
private:
    RiskService riskService;
    UserDAO userDAO;
public:
METHOD_LIST_BEGIN
ADD_METHOD_TO(
    RiskController::checkRisk,
    "/api/risk/check",
    drogon::Post
);
METHOD_LIST_END
void checkRisk(
    const drogon::HttpRequestPtr& req,
    std::function<void(
        const drogon::HttpResponsePtr&
    )>&& callback
);
};