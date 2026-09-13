#include "UserDAO.h"
#include <iostream>
bool UserDAO::insertUser(const User& user)
{
    std::cout << "DAO: 插入用户 " << user.username << std::endl;
    return true;
}
bool UserDAO::existsByUsername(const std::string& username)
{
    std::cout << "DAO: 查询用户名 " << username << std::endl;
    if (username == "Tom")
    {
        return true;
    }
    return false;
}