cmake_minimum_required(VERSION 3.20)

project(personal_loan_backend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_executable(
    personal_loan_backend
    src/main.cpp
    controller/UserController.cpp
    service/UserService.cpp
    dao/UserDAO.cpp
    )