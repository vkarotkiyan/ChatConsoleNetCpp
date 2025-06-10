#pragma once
#include <string>
#include <iostream>
#include <vector>
#if defined (_WIN64) || defined(_WIN32)
#include <mysql.h>
#elif defined (__linux__)
#include <mysql/mysql.h>
#endif
#include "db.h"

class DB_MySQL : public DB {
public:
	DB_MySQL(std::string ip, int port, std::string db_name, std::string db_login, std::string db_pass) :
		m_ip(ip), m_port(port), m_db_name(db_name), m_db_login(db_login), m_db_pass(db_pass) { };
	~DB_MySQL();
	void db_start();
	void sendRequest(const std::string& request); // Запись данных в базу (INSERT, UPDATE)
	std::vector<std::string> getRequest(const std::string& request, int& num_rows, int& num_fields); // Чтение данных из базы
private:
	void initdb();
	MYSQL mysql;
	MYSQL_RES* m_result;
	MYSQL_ROW m_row;
	std::string m_query;
	std::string m_ip;
	int m_port;
	std::string m_db_name;
	std::string m_db_login;
	std::string m_db_pass;
};
