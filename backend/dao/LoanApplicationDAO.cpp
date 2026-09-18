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
){
    MYSQL* conn=connectDatabase();
    if(conn==nullptr)
        return false;
    std::string sql =
    "SELECT id,user_id,loan_id,amount,status,created_at "
    "FROM loan_application WHERE user_id="
    + std::to_string(userId);

    if(mysql_query(conn,sql.c_str())!=0)
    {
        mysql_close(conn);
        return false;
    }
    MYSQL_RES* result=mysql_store_result(conn);
    MYSQL_ROW row;
    while((row=mysql_fetch_row(result)))
    {
        LoanApplication app;
        app.id=std::stoi(row[0]);
        app.userId=std::stoi(row[1]);
        app.loanId=std::stoi(row[2]);
        app.amount=std::stod(row[3]);
        app.status=row[4];
        applications.push_back(app);
    }
    mysql_free_result(result);
    mysql_close(conn);
    return true;
}