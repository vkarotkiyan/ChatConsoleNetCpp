#pragma once
#include <string>
#include <vector>

class DB { // Базовый класс для работы с базой данных
public:
    virtual ~DB() {};
    virtual void db_start() = 0;
    virtual void sendRequest(const std::string& query) = 0;
    virtual std::vector<std::string> getRequest(const std::string& request, int& num_rows, int& num_fields) = 0;
};
