#pragma once
#include "../model/User.h"
#include "../dao/UserDAO.h"
#include<string>
class UserService
{
private:
    UserDAO userDAO;
public:
    bool registerUser(const User& user);
    bool loginUser(const std::string& username,
    const std::string& password);
    bool getUserById(int id, User& user);
    bool updateUser(const User& user);
};