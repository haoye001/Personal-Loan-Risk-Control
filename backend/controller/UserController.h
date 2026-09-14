#pragma once
#include "../model/User.h"
#include "../service/UserService.h"
#include <string>
class UserController
{
private:
    UserService userService;

public:
    void registerUser(const User& user);
    bool loginUser(const std::string& username,
    const std::string& password);
    bool getUserById(int id, User& user);
    bool updateUser(const User& user);
};