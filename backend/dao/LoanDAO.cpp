#include "LoanDAO.h"
#include <mysql.h>
#include <iostream>
bool LoanDAO::getAllLoans(std::vector<Loan>& loans)
{
    MYSQL* conn = mysql_init(nullptr);
    if(conn == nullptr)
    {
        std::cerr << "mysql_init failed" << std::endl;
        return false;
    }
    if(mysql_real_connect(
        conn,
        "localhost",
        "root",
        "061105",
        "personal_loans",
        3307,
        nullptr,
        0
    ) == nullptr)
    {
        std::cerr << "database connection failed: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        return false;
    }
    const char* sql =
        "SELECT id, name, max_amount, term "
        "FROM loan_product";

    if(mysql_query(conn, sql) != 0)
    {
        std::cerr << "query loan products failed: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        return false;
    }
    MYSQL_RES* result = mysql_store_result(conn);

    if(result == nullptr)
    {
        std::cerr << "get result failed: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        return false;
    }
    MYSQL_ROW row;
    while((row = mysql_fetch_row(result)) != nullptr)
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
    return true;
}