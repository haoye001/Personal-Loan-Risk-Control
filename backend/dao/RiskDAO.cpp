#include "RiskDAO.h"
#include <iostream>
#include <mysql.h>
// 建立 MySQL 连接
MYSQL* connectRiskDatabase()
{
    MYSQL* conn = mysql_init(nullptr);
    if (conn == nullptr)
    {
        std::cout << "RiskDAO: mysql_init failed"
                  << std::endl;
        return nullptr;
    }
    mysql_options(
        conn,
        MYSQL_OPT_CONNECT_TIMEOUT,
        "5"
    );
    if (mysql_real_connect(
            conn,
            "127.0.0.1",
            "root",
            "061105",
            "personal_loans",
            3307,
            nullptr,
            0) == nullptr)
    {
        std::cout << "RiskDAO: MySQL connection failed: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        return nullptr;
    }
    return conn;
}
// 保存风险结果
bool RiskDAO::saveRiskResult(const RiskResult& result)
{
    MYSQL* conn = connectRiskDatabase();
    if (conn == nullptr)
    {
        return false;
    }
    std::string sql =
        "INSERT INTO risk_result "
        "(application_id, score, level, decision) VALUES ("
        + std::to_string(result.applicationId)
        + ", "
        + std::to_string(result.score)
        + ", '"
        + result.level
        + "', '"
        + result.decision
        + "')";
    std::cout << "RISK SQL: "
              << sql
              << std::endl;
    if (mysql_query(conn, sql.c_str()) != 0)
    {
        std::cout << "INSERT risk result failed: "
                  << mysql_error(conn)
                  << std::endl;
        mysql_close(conn);
        return false;
    }
    std::cout << "RiskDAO: save risk result success"
              << std::endl;
    mysql_close(conn);
    return true;
}
bool RiskDAO::getRiskResultByApplicationId(
    int applicationId,
    RiskResult& result
)
{
    MYSQL* conn = connectRiskDatabase();
    if (conn == nullptr)
    {
        return false;
    }
    std::string sql =
        "SELECT application_id, score, level, decision "
        "FROM risk_result "
        "WHERE application_id = "
        + std::to_string(applicationId)
        + " ORDER BY id DESC LIMIT 1";
    std::cout << "RISK QUERY SQL: "
              << sql
              << std::endl;
    if (mysql_query(conn, sql.c_str()) != 0)
    {
        std::cout << "SELECT risk result failed: "
                  << mysql_error(conn)
                  << std::endl;
        mysql_close(conn);
        return false;
    }
    MYSQL_RES* queryResult = mysql_store_result(conn);
    if (queryResult == nullptr)
    {
        std::cout << "Get risk result failed: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        return false;
    }
    MYSQL_ROW row = mysql_fetch_row(queryResult);
    if (row == nullptr)
    {
        mysql_free_result(queryResult);
        mysql_close(conn);
        return false;
    }
    result.applicationId = std::stoi(row[0]);
    result.score = std::stoi(row[1]);
    result.level = row[2];
    result.decision = row[3];
    mysql_free_result(queryResult);
    mysql_close(conn);
    return true;
}
bool RiskDAO::updateRiskDecision(
    int applicationId,
    const std::string& decision
)
{
    MYSQL* conn = mysql_init(nullptr);

    if (conn == nullptr)
    {
        return false;
    }

    if (!mysql_real_connect(
            conn,
            "localhost",
            "root",
            "061105",
            "personal_loans",
            3307,
            nullptr,
            0))
    {
        mysql_close(conn);
        return false;
    }

    std::string sql =
        "UPDATE risk_result "
        "SET decision = '" + decision + "' "
        "WHERE id = ("
        "SELECT id FROM ("
        "SELECT id FROM risk_result "
        "WHERE application_id = " +
        std::to_string(applicationId) +
        " ORDER BY id DESC LIMIT 1"
        ") AS latest"
        ")";

    bool success =
        mysql_query(conn, sql.c_str()) == 0;

    mysql_close(conn);

    return success;
}