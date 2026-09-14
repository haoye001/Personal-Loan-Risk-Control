#include "dao/UserDAO.h"

int main()
{
    UserDAO dao;
    return dao.existsByUsername("Jerry") ? 0 : 1;
}
