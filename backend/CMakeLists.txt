cmake_minimum_required(VERSION 3.20)

project(personal_loan_backend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# MySQL
set(MYSQL_INCLUDE_DIR "C:/Program Files/MySQL/MySQL Server 8.0/include")
set(MYSQL_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/libmysql.dll.a")
add_executable(
    personal_loan_backend

    src/main.cpp

    controller/UserController.cpp
    service/UserService.cpp
    dao/UserDAO.cpp
)

target_include_directories(
    personal_loan_backend
    PRIVATE
    ${MYSQL_INCLUDE_DIR}
)

target_link_libraries(
    personal_loan_backend
    PRIVATE
    ${MYSQL_LIBRARY}
)