#pragma once
#include "db_mysql.h"
#if defined(_WIN64) || defined(_WIN32)
#include "db_myodbc.h"
#endif

class DB_Interface { // Класс выбора интерфейса базы данных
public:
    static DB* DBInitInterface(
        const std::string ip, 
        const int port,
        const std::string db_name,
        const std::string db_login,
        const std::string db_pass,
        const std::string method
    ) {
#if defined(_WIN64) || defined(_WIN32)
        if (method == "mysqlapi")
            return new DB_MySQL(ip, port, db_name, db_login, db_pass);
        else if (method == "myodbc")
            return new DB_MyODBC(ip, port, db_name, db_login, db_pass);
        return nullptr;
#elif defined (__linux__)
        return new DB_MySQL(ip, port, db_name, db_login, db_pass);
#endif
    }
};
