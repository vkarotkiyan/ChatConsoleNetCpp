#if defined(_WIN64) || defined(_WIN32)
#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <windows.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
#include "db.h"

class DB_MyODBC : public DB { // Интерфейс ODBC
public:
	DB_MyODBC(std::string ip, int port, std::string db_name, std::string db_login, std::string db_pass) :
		m_ip(ip), m_port(port), m_db_name(db_name), m_db_login(db_login), m_db_pass(db_pass) { };
	~DB_MyODBC();
	void db_start();
	void sendRequest(const std::string& request); // Запись данных в базу (INSERT, UPDATE)
	std::vector<std::string> getRequest(const std::string& request, int& num_rows, int& num_fields); // Чтение данных из базы
private:
	void initdb();
	SQLHANDLE sqlConn{ nullptr }; // дескриптор для соединения с базой данных
	SQLHANDLE sqlEnv{ nullptr }; // дескриптор окружения базы данных
	SQLHANDLE sqlStmt{ nullptr };  // дескриптор для выполнения запросов к базе данных

	SQLRETURN m_ret;
	SQLLEN m_rows = 0;
	SQLSMALLINT m_fields = 0;
	std::string m_query;
	
	std::string m_ip;
	int m_port;
	std::string m_db_name;
	std::string m_db_login;
	std::string m_db_pass;
};
#endif