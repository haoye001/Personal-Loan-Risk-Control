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
bool UserController::loginUser(
    const std::string& username,
    const std::string& password
){
    if (userService.loginUser(username, password))
    {
        std::cout << "Controller: Login success"
                  << std::endl;

        return true;
    }

    std::cout << "Controller: Login failed"
              << std::endl;

    return false;    
}
bool UserController::getUserById(int id, User& user)
{
    if (userService.getUserById(id, user))
    {
        std::cout << "Controller: Get user success"
                  << std::endl;

        std::cout << "id: " << user.id << std::endl;
        std::cout << "username: " << user.username << std::endl;
        std::cout << "phone: " << user.phone << std::endl;

        return true;
    }

    std::cout << "Controller: User not found"
              << std::endl;

    return false;
}
bool UserController::updateUser(const User& user)
{
    if (userService.updateUser(user))
    {
        std::cout << "Controller: user update success"
                  << std::endl;

        return true;
    }

    std::cout << "Controller: user update failed"
              << std::endl;

    return false;
}