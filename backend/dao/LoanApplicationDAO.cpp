#include "LoanApplicationDAO.h"
#include <mysql.h>
#include <iostream>
MYSQL* connectDatabase();
bool LoanApplicationDAO::insertApplication(
    const LoanApplication& application
){
    MYSQL* conn = connectDatabase();
    if(conn == nullptr)
    {
        return false;
    }
    std::string sql =
        "INSERT INTO loan_application "
        "(user_id, loan_id, amount) VALUES ("
        + std::to_string(application.userId)
        + ","
        + std::to_string(application.loanId)
        + ","
        + std::to_string(application.amount)
        + ")";
    std::cout 
        << "INSERT SQL: "
        << sql
        << std::endl;
    if(mysql_query(conn, sql.c_str()) != 0)
    {
        std::cout
            << "INSERT application failed: "
            << mysql_error(conn)
            << std::endl;
        mysql_close(conn);
        return false;
    }
    std::cout
        << "DAO: application insert success"
        << std::endl;
    mysql_close(conn);
    return true;
}
bool LoanApplicationDAO::getApplicationsByUserId(
    int userId,
    std::vector<LoanApplication>& applications
)
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
        std::cout
            << "SELECT applications failed: "
            << mysql_error(conn)
            << std::endl;
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
        // 风险评估结果可能不存在，因此需要判断 NULL
        app.riskScore = row[6] ? std::stoi(row[6]) : 0;
        app.riskLevel = row[7] ? row[7] : "";
        app.decision = row[8] ? row[8] : "";
        applications.push_back(app);
    }
    mysql_free_result(result);
    mysql_close(conn);
    return true;
}
bool LoanApplicationDAO::updateApplicationStatus(
    int applicationId,
    const std::string& status
)
{
    MYSQL* conn = connectDatabase();

    if (conn == nullptr)
    {
        return false;
    }

    std::string sql =
        "UPDATE loan_application "
        "SET status = '" + status + "' "
        "WHERE id = " + std::to_string(applicationId);

    if (mysql_query(conn, sql.c_str()) != 0)
    {
        std::cout
            << "UPDATE application status failed: "
            << mysql_error(conn)
            << std::endl;

        mysql_close(conn);
        return false;
    }

    mysql_close(conn);
    return true;
}
bool LoanApplicationDAO::getApplicationById(
    int applicationId,
    LoanApplication& application
)
{
    MYSQL* conn = connectDatabase();

    if (conn == nullptr)
    {
        return false;
    }

    std::string sql =
        "SELECT id, user_id, loan_id, amount, status "
        "FROM loan_application "
        "WHERE id = "
        + std::to_string(applicationId);

    if (mysql_query(conn, sql.c_str()) != 0)
    {
        std::cout
            << "SELECT application failed: "
            << mysql_error(conn)
            << std::endl;

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

    mysql_free_result(result);
    mysql_close(conn);

    return true;
}