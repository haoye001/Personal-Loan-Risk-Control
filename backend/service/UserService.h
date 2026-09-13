#pragma once
#include "../model/User.h"
#include "../dao/UserDAO.h"
class UserService
{
private:
    UserDAO userDAO;
public:
    bool registerUser(const User& user);
};