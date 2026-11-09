#include "LoanDAO.h"
#include "../utils/Logger.h"
#include <mysql.h>
#include <iostream>

namespace
{
    // 局部连接辅助函数
    MYSQL* connectLoanDatabase()
    {
        MYSQL* conn = mysql_init(nullptr);
        if (conn == nullptr)
        {
            LOG_ERROR("LoanDAO: mysql_init failed");
            return nullptr;
        }

        mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, "5");

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
            LOG_ERROR(std::string("LoanDAO: MySQL connection failed: ")
                      + mysql_error(conn));
            mysql_close(conn);
            return nullptr;
        }

        return conn;
    }
}

bool LoanDAO::getAllLoans(std::vector<Loan>& loans)
{
    MYSQL* conn = connectLoanDatabase();
    if (conn == nullptr)
    {
        return false;
    }

    const char* sql =
        "SELECT id, name, max_amount, term "
        "FROM loan_product "
        "ORDER BY id";

    if (mysql_query(conn, sql) != 0)
    {
        LOG_ERROR(std::string("LoanDAO: query loan products failed: ")
                  + mysql_error(conn));
        mysql_close(conn);
        return false;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == nullptr)
    {
        LOG_ERROR(std::string("LoanDAO: get result failed: ")
                  + mysql_error(conn));
        mysql_close(conn);
        return false;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)) != nullptr)
    {
        Loan loan;
        loan.id = std::stoi(row[0]);
        loan.name = row[1];
        loan.maxAmount = std::stod(row[2]);
        loan.term = std::stoi(row[3]);
        loans.push_back(loan);
    }

    mysql_free_result(result);
    mysql_close(conn);

    LOG_INFO("LoanDAO: getAllLoans, count = " + std::to_string(loans.size()));
    return true;
}

bool LoanDAO::getLoanById(int id, Loan& loan)
{
    MYSQL* conn = connectLoanDatabase();
    if (conn == nullptr)
    {
        return false;
    }

    std::string sql =
        "SELECT id, name, max_amount, term "
        "FROM loan_product "
        "WHERE id = " + std::to_string(id);

    if (mysql_query(conn, sql.c_str()) != 0)
    {
        LOG_ERROR(std::string("LoanDAO: getLoanById failed: ")
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

    loan.id = std::stoi(row[0]);
    loan.name = row[1];
    loan.maxAmount = std::stod(row[2]);
    loan.term = std::stoi(row[3]);

    mysql_free_result(result);
    mysql_close(conn);

    LOG_INFO("LoanDAO: getLoanById id = " + std::to_string(id));
    return true;
}

bool LoanDAO::createLoan(const Loan& loan)
{
    MYSQL* conn = connectLoanDatabase();
    if (conn == nullptr)
    {
        return false;
    }

    std::string sql =
        "INSERT INTO loan_product (name, max_amount, term) VALUES ('"
        + loan.name + "', "
        + std::to_string(loan.maxAmount) + ", "
        + std::to_string(loan.term) + ")";

    if (mysql_query(conn, sql.c_str()) != 0)
    {
        LOG_ERROR(std::string("LoanDAO: createLoan failed: ")
                  + mysql_error(conn));
        mysql_close(conn);
        return false;
    }

    LOG_INFO("LoanDAO: createLoan name = " + loan.name);
    mysql_close(conn);
    return true;
}

bool LoanDAO::updateLoan(const Loan& loan)
{
    MYSQL* conn = connectLoanDatabase();
    if (conn == nullptr)
    {
        return false;
    }

    std::string sql =
        "UPDATE loan_product SET name = '"
        + loan.name + "', max_amount = "
        + std::to_string(loan.maxAmount) + ", term = "
        + std::to_string(loan.term) + " "
        "WHERE id = " + std::to_string(loan.id);

    if (mysql_query(conn, sql.c_str()) != 0)
    {
        LOG_ERROR(std::string("LoanDAO: updateLoan failed: ")
                  + mysql_error(conn));
        mysql_close(conn);
        return false;
    }

    my_ulonglong affectedRows = mysql_affected_rows(conn);

    mysql_close(conn);

    if (affectedRows == 0)
    {
        LOG_WARN("LoanDAO: updateLoan no affected rows, id = "
                 + std::to_string(loan.id));
        return false;
    }

    LOG_INFO("LoanDAO: updateLoan id = " + std::to_string(loan.id));
    return true;
}
