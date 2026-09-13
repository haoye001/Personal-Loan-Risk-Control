#pragma once
#include "../model/User.h"
#include "../service/UserService.h"
class UserController
{
private:
    UserService userService;

public:
    void registerUser(const User& user);
};