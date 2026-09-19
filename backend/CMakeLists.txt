cmake_minimum_required(VERSION 3.20)

project(personal_loan_backend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==============================
# Drogon
# ==============================
find_package(Drogon REQUIRED)

# ==============================
# MySQL
# ==============================
set(MYSQL_INCLUDE_DIR "" CACHE PATH "MySQL include directory")
set(MYSQL_LIBRARY "" CACHE FILEPATH "MySQL client library")

if(NOT MYSQL_INCLUDE_DIR)
    message(FATAL_ERROR
        "MYSQL_INCLUDE_DIR is not set. "
        "Please specify the MySQL include directory when configuring CMake."
    )
endif()

if(NOT MYSQL_LIBRARY)
    message(FATAL_ERROR
        "MYSQL_LIBRARY is not set. "
        "Please specify the MySQL client library when configuring CMake."
    )
endif()

# ==============================
# Executable
# ==============================
add_executable(
    personal_loan_backend

    src/main.cpp

    controller/UserController.cpp
    service/UserService.cpp
    dao/UserDAO.cpp
    controller/LoanController.cpp
    controller/LoanApplicationController.cpp
    controller/RiskController.cpp
    service/LoanService.cpp
    dao/LoanDAO.cpp
    dao/RiskDAO.cpp
    dao/LoanApplicationDAO.cpp
    service/LoanApplicationService.cpp
    service/RiskService.cpp
)

# ==============================
# Include directories
# ==============================
target_include_directories(
    personal_loan_backend
    PRIVATE
    ${MYSQL_INCLUDE_DIR}
)

# ==============================
# Libraries
# ==============================
target_link_libraries(
    personal_loan_backend
    PRIVATE
    ${MYSQL_LIBRARY}
    Drogon::Drogon
)