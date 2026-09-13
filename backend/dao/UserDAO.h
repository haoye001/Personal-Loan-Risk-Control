#pragma once
#include <string>
#include "../model/User.h"
class UserDAO
{
public:
    bool insertUser(const User& user);
    bool existsByUsername(const std::string& username);
};