#include "LoanApplicationDAO.h"
#include "../utils/Logger.h"
#include <mysql.h>
#include <iostream>

// 外部声明：UserDAO.cpp 中定义的 connectDatabase()
extern MYSQL* connectDatabase();

namespace
{
    bool executeUpdateStatus(
        MYSQL* conn,
        int applicationId,
        const std::string& status)
    {
        std::string sql =
            "UPDATE loan_application "
            "SET status = '" + status + "' "
            "WHERE id = " + std::to_string(applicationId);

        if (mysql_query(conn, sql.c_str()) != 0)
        {
            LOG_ERROR(std::string("LoanApplicationDAO: update status failed: ")
                      + mysql_error(conn));
            return false;
        }

        return mysql_affected_rows(conn) > 0;
    }
}

bool LoanApplicationDAO::insertApplication(
    const LoanApplication& application,
    int& newId)
{
    MYSQL* conn = connectDatabase();
    if (conn == nullptr)
    {
        return false;
    }

    std::string sql =
        "INSERT INTO loan_application "
        "(user_id, loan_id, amount, status) VALUES ("
        + std::to_string(application.userId) + ", "
        + std::to_string(application.loanId) + ", "
        + std::to_string(application.amount) + ", 'pending')";

    LOG_INFO("LoanApplicationDAO: " + sql);

    if (mysql_query(conn, sql.c_str()) != 0)
    {
        LOG_ERROR(std::string("LoanApplicationDAO: insert application failed: ")
                  + mysql_error(conn));
        mysql_close(conn);
        return false;
    }

    newId = static_cast<int>(mysql_insert_id(conn));

    LOG_INFO("LoanApplicationDAO: application insert success, id = "
             + std::to_string(newId));

    mysql_close(conn);
    return true;
}

bool LoanApplicationDAO::getApplicationsByUserId(
    int userId,
    std::vector<LoanApplication>& applications)
{
    MYSQL* conn = connectDatabase();
    if (conn == nullptr)
    {
        return false;
    }

    std::string sql =
        "SELECT "
        "la.id, "
        "la.user_id, "
        "la.loan_id, "
        "la.amount, "
        "la.status, "
        "la.created_at, "
        "rr.score, "
        "rr.level, "
        "rr.decision "
        "FROM loan_application la "
        "LEFT JOIN risk_result rr "
        "ON rr.id = ("
        "    SELECT r2.id "
        "    FROM risk_result r2 "
        "    WHERE r2.application_id = la.id "
        "    ORDER BY r2.id DESC "
        "    LIMIT 1"
        ") "
        "WHERE la.user_id = "
        + std::to_string(userId)
        + " ORDER BY la.id DESC";

    if (mysql_query(conn, sql.c_str()) != 0)
    {
        LOG_ERROR(std::string("LoanApplicationDAO: select applications failed: ")
                  + mysql_error(conn));
        mysql_close(conn);
        return false;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr)
    {
        mysql_close(conn);
        return false;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)))
    {
        LoanApplication app;
        app.id = std::stoi(row[0]);
        app.userId = std::stoi(row[1]);
        app.loanId = std::stoi(row[2]);
        app.amount = std::stod(row[3]);
        app.status = row[4] ? row[4] : "";
        app.createdAt = row[5] ? row[5] : "";
        app.riskScore = row[6] ? std::stoi(row[6]) : 0;
        app.riskLevel = row[7] ? row[7] : "";
        app.decision = row[8] ? row[8] : "";
        applications.push_back(app);
    }

    mysql_free_result(result);
    mysql_close(conn);

    LOG_INFO("LoanApplicationDAO: getApplicationsByUserId userId = "
             + std::to_string(userId)
             + ", count = "
             + std::to_string(applications.size()));

    return true;
}

bool LoanApplicationDAO::getApplicationById(
    int applicationId,
    LoanApplication& application)
{
    MYSQL* conn = connectDatabase();
    if (conn == nullptr)
    {
        return false;
    }

    std::string sql =
        "SELECT id, user_id, loan_id, amount, status, created_at "
        "FROM loan_application "
        "WHERE id = "
        + std::to_string(applicationId);

    if (mysql_query(conn, sql.c_str()) != 0)
    {
        LOG_ERROR(std::string("LoanApplicationDAO: select application failed: ")
                  + mysql_error(conn));
        mysql_close(conn);
        return false;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr)
    {
        mysql_close(conn);
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);

    if (row == nullptr)
    {
        mysql_free_result(result);
        mysql_close(conn);
        return false;
    }

    application.id = std::stoi(row[0]);
    application.userId = std::stoi(row[1]);
    application.loanId = std::stoi(row[2]);
    application.amount = std::stod(row[3]);
    application.status = row[4] ? row[4] : "";
    application.createdAt = row[5] ? row[5] : "";

    mysql_free_result(result);
    mysql_close(conn);

    return true;
}

bool LoanApplicationDAO::updateApplicationStatus(
    int applicationId,
    const std::string& status)
{
    MYSQL* conn = connectDatabase();
    if (conn == nullptr)
    {
        return false;
    }

    bool success = executeUpdateStatus(conn, applicationId, status);
    mysql_close(conn);

    return success;
}

bool LoanApplicationDAO::hasPendingApplication(
    int userId,
    int loanId)
{
    MYSQL* conn = connectDatabase();
    if (conn == nullptr)
    {
        return false;
    }

    std::string sql =
        "SELECT id FROM loan_application "
        "WHERE user_id = " + std::to_string(userId) +
        " AND loan_id = " + std::to_string(loanId) +
        " AND status NOT IN ('rejected', 'completed')";

    if (mysql_query(conn, sql.c_str()) != 0)
    {
        LOG_ERROR(std::string("LoanApplicationDAO: hasPendingApplication failed: ")
                  + mysql_error(conn));
        mysql_close(conn);
        return false;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr)
    {
        mysql_close(conn);
        return false;
    }

    bool exists = mysql_num_rows(result) > 0;

    mysql_free_result(result);
    mysql_close(conn);

    return exists;
}

bool LoanApplicationDAO::transitionApplicationStatus(
    int applicationId,
    const std::string& fromStatus,
    const std::string& toStatus)
{
    MYSQL* conn = connectDatabase();
    if (conn == nullptr)
    {
        return false;
    }

    // 开启事务
    mysql_query(conn, "START TRANSACTION");

    // 先校验当前状态是否匹配
    std::string verifySql =
        "SELECT status FROM loan_application WHERE id = "
        + std::to_string(applicationId)
        + " FOR UPDATE";

    if (mysql_query(conn, verifySql.c_str()) != 0)
    {
        LOG_ERROR(std::string("LoanApplicationDAO: transition verify failed: ")
                  + mysql_error(conn));
        mysql_query(conn, "ROLLBACK");
        mysql_close(conn);
        return false;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr)
    {
        mysql_query(conn, "ROLLBACK");
        mysql_close(conn);
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == nullptr)
    {
        mysql_free_result(result);
        mysql_query(conn, "ROLLBACK");
        mysql_close(conn);
        return false;
    }

    std::string currentStatus = row[0] ? row[0] : "";
    mysql_free_result(result);

    if (currentStatus != fromStatus)
    {
        LOG_WARN("LoanApplicationDAO: status mismatch, expected "
                 + fromStatus + " but got " + currentStatus);
        mysql_query(conn, "ROLLBACK");
        mysql_close(conn);
        return false;
    }

    bool success = executeUpdateStatus(conn, applicationId, toStatus);

    if (success)
    {
        mysql_query(conn, "COMMIT");
        LOG_INFO("LoanApplicationDAO: transition "
                 + fromStatus + " -> " + toStatus
                 + " for application " + std::to_string(applicationId));
    }
    else
    {
        mysql_query(conn, "ROLLBACK");
    }

    mysql_close(conn);

    return success;
}
