#include <iostream>
#include <mysql.h>

int main()
{
    std::cout << "TEST START" << std::endl;

    MYSQL* conn = mysql_init(nullptr);

    if (conn == nullptr)
    {
        std::cout << "mysql_init failed" << std::endl;
        return 1;
    }

    std::cout << "mysql_init success" << std::endl;

    mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, "5");

    std::cout << "before mysql_real_connect" << std::endl;

    MYSQL* result = mysql_real_connect(
        conn,
        "127.0.0.1",
        "root",
        "061105",
        "personal_loans",
        3307,
        nullptr,
        0
    );

    std::cout << "after mysql_real_connect" << std::endl;

    if (result == nullptr)
    {
        std::cout << "CONNECT FAILED: "
                  << mysql_error(conn)
                  << std::endl;

        mysql_close(conn);
        return 1;
    }

    std::cout << "CONNECT SUCCESS!" << std::endl;

    mysql_close(conn);

    return 0;
}