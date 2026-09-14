#pragma once
#include <string>
#include "../model/User.h"
class UserDAO
{
public:
    bool insertUser(const User& user);
    bool existsByUsername(const std::string& username);
    bool checkLogin(const std::string& username,
        const std::string& password);
    bool getUserById(int id, User& user);
    bool updateUser(const User& user);
};