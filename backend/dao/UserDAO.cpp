#include "UserDAO.h"

#include <iostream>
#include <mysql.h>

// MySQL 配置
const char* HOST = "127.0.0.1";
const char* USER = "root";
const char* PASSWORD = "061105";
const char* DATABASE = "personal_loans";
const unsigned int PORT = 3307;


// 建立 MySQL 连接
MYSQL* connectDatabase()
{
    std::cout << "DAO: start connecting MySQL..." << std::endl;

    MYSQL* conn = mysql_init(nullptr);

    if (conn == nullptr)
    {
        std::cout << "mysql_init failed" << std::endl;
        return nullptr;
    }

    mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, "5");

    std::cout << "DAO: before mysql_real_connect" << std::endl;

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
        std::cout << "MySQL connection failed: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        return nullptr;
    }

    std::cout << "DAO: MySQL connection success!" << std::endl;

    return conn;
}
// 插入用户
bool UserDAO::insertUser(const User& user)
{
    MYSQL* conn = connectDatabase();

    if (conn == nullptr)
    {
        return false;
    }

    std::string sql =
        "INSERT INTO user (username, password, phone) VALUES ('"
        + user.username + "', '"
        + user.password + "', '"
        + user.phone + "')";

    if (mysql_query(conn, sql.c_str()) != 0)
    {
        std::cout << "INSERT 失败: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        return false;
    }

    std::cout << "DAO: 用户插入成功" << std::endl;

    mysql_close(conn);

    return true;
}


// 查询用户名是否存在
bool UserDAO::existsByUsername(const std::string& username)
{
    MYSQL* conn = connectDatabase();

    if (conn == nullptr)
    {
        return false;
    }

    std::string sql =
        "SELECT id FROM user WHERE username = '"
        + username + "'";

    if (mysql_query(conn, sql.c_str()) != 0)
    {
        std::cout << "SELECT 失败: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        return false;
    }

    MYSQL_RES* result = mysql_store_result(conn);

    if (result == nullptr)
    {
        std::cout << "获取查询结果失败: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        return false;
    }

    bool exists = mysql_num_rows(result) > 0;

    mysql_free_result(result);
    mysql_close(conn);

    return exists;
}
bool UserDAO::checkLogin(const std::string& username,const std::string& password){
    MYSQL* conn=connectDatabase();
    if(conn==nullptr){
        return false;
    }
    std::string sql=
        "SELECT id FROM user WHERE username = '"
        + username
        + "' AND password = '"
        + password
        + "'";
        if(mysql_query(conn,sql.c_str())!=0){
        std::cout << "SELECT login failed: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        return false;
        } 
        MYSQL_RES* result=mysql_store_result(conn);
        if(result==nullptr){
        std::cout << "Get login result failed: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        return false;
        }   
        bool success=mysql_num_rows(result)>0;
        mysql_free_result(result);
        mysql_close(conn);
        return success;
}
bool UserDAO::getUserById(int id, User& user)
{
    MYSQL* conn = connectDatabase();

    if (conn == nullptr)
    {
        return false;
    }

    std::string sql =
        "SELECT id, username, password, phone, "
        "age, income, job, debt "
        "FROM `user` WHERE id = "
        + std::to_string(id);
    std::cout
    << "USER SQL: "
    << sql
    << std::endl;
    if (mysql_query(conn, sql.c_str()) != 0)
    {
        std::cout << "SELECT user failed: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        return false;
    }

    MYSQL_RES* result = mysql_store_result(conn);

    if (result == nullptr)
    {
        std::cout << "Get user result failed: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);

    if (row == nullptr)
    {
        std::cout
        << "DAO: user not found"
        << std::endl;
        mysql_free_result(result);
        mysql_close(conn);
        return false;
    }
    std::cout
    << "DAO: find user id="
    << row[0]
    << std::endl;
    user.id = std::stoi(row[0]);
    user.username = row[1];
    user.password = row[2];
    user.phone = row[3] ? row[3] : "";
    user.age = row[4] ? std::stoi(row[4]) : 0;
    user.income = row[5] ? std::stod(row[5]) : 0;
    user.job = row[6] ? row[6] : "";
    user.debt = row[7] ? std::stod(row[7]) : 0;
    mysql_free_result(result);
    mysql_close(conn);

    return true;
}
bool UserDAO::updateUser(const User& user)
{
    MYSQL* conn = connectDatabase();

    if (conn == nullptr)
    {
        std::cout << "UPDATE: database connection failed"
                  << std::endl;

        return false;
    }

    std::string sql =
        "UPDATE user SET username = '"
        + user.username
        + "', phone = '"
        + user.phone
        + "' WHERE id = "
        + std::to_string(user.id);

    std::cout << "UPDATE SQL: "
              << sql
              << std::endl;

    if (mysql_query(conn, sql.c_str()) != 0)
    {
        std::cout << "UPDATE failed: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        return false;
    }

    std::cout << "UPDATE query executed successfully"
              << std::endl;

    my_ulonglong affectedRows =
        mysql_affected_rows(conn);

    std::cout << "UPDATE affected rows: "
              << affectedRows
              << std::endl;

    mysql_close(conn);

    if (affectedRows == 0)
    {
        std::cout << "UPDATE: no user was updated"
                  << std::endl;

        return false;
    }

    std::cout << "DAO: user update success"
              << std::endl;

    return true;
}