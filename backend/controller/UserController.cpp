#include "UserController.h"
#include <iostream>
void UserController::registerUser(const User& user)
{
    if (userService.registerUser(user))
    {
        std::cout << "Controller: 注册成功" << std::endl;
    }
    else
    {
        std::cout << "Controller: 用户名已存在" << std::endl;
    }
}