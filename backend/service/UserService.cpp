#include "UserService.h"
bool UserService::registerUser(const User& user){
    //检查用户名是否存在
    if(userDAO.existsByUsername(user.username)){
        return false;
    }
    return userDAO.insertUser(user);
}