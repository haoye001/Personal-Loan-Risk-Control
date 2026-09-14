#include <iostream>

#include "../controller/UserController.h"

int main()
{
    UserController controller;

    User user;

    user.id = 1;
    user.username = "Jerry";
    user.phone = "222222222";

    controller.updateUser(user);

    return 0;
}