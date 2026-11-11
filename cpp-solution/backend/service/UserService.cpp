#include "UserService.h"
bool UserService::registerUser(const User& user){
    //检查用户名是否存在
    if(userDAO.existsByUsername(user.username)){
        return false;
    }
    return userDAO.insertUser(user);
}
bool UserService::loginUser(
    const std::string& username,
    const std::string& password
){
    return userDAO.checkLogin(username,password);
}
bool UserService::getUserById(int id, User& user)
{
    return userDAO.getUserById(id, user);
}
bool UserService::updateUser(const User& user)
{
    return userDAO.updateUser(user);
}