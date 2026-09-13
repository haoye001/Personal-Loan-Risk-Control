#include <iostream>
#include "../model/User.h"
#include "../controller/UserController.h"

int main()
{
    User user;
    user.id = 0;
    user.username = "Jerry";
    user.password = "123456";
    user.phone = "13800138000";

    UserController controller;

    controller.registerUser(user);

    return 0;
}