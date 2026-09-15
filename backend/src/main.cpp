#include <iostream>

#include "../controller/UserController.h"
#include <drogon/drogon.h>

int main()
{
    std::cout << "starting server" << std::endl;

    drogon::app()
        .addListener("0.0.0.0", 8888)
        .run();

    return 0;
}