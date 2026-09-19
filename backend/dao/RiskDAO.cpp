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