#if defined(_WIN64) || defined(_WIN32)
#include "db_myodbc.h"
using namespace  std;

DB_MyODBC::~DB_MyODBC() {
	SQLFreeHandle(SQL_HANDLE_STMT, sqlStmt);
	SQLDisconnect(sqlConn);
	SQLFreeHandle(SQL_HANDLE_DBC, sqlConn);
	SQLFreeHandle(SQL_HANDLE_ENV, sqlEnv);
	cout << "Соединение с базой данных закрыто." << endl;
}

void DB_MyODBC::db_start() {
	constexpr auto SQL_RETURN_CODE_LEN = 1024;
	SQLCHAR retconstring[SQL_RETURN_CODE_LEN]{}; // строка для кода возврата из функций API ODBC
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnv)) {
		cout << "Ошибка инициализации среды" << endl;
		exit(1);
	}
	if (SQL_SUCCESS != SQLSetEnvAttr(sqlEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0)) {
		cout << "Ошибка установки версии ODBC" << endl;
		exit(1);
	}
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, sqlEnv, &sqlConn)) {
		cout << "Ошибка выделения дескриптора подключения" << endl;
		exit(1);
	}
	// Устанавливаем соединение с сервером
	string connStr = "DRIVER={MySQL ODBC 9.3 Unicode Driver};SERVER=" + m_ip + ";PORT=" + to_string(m_port) + 
		//";DATABASE=" + m_db_name + 
		";UID=" + m_db_login + 
		";PWD=" + m_db_pass + 
		";CHARSET=utf8mb4;OPTION=3;LINGUISTIC_SETTINGS=utf8mb4_general_ci;";
	m_ret = SQLDriverConnectA(sqlConn, NULL, (SQLCHAR*)connStr.c_str(), SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
	if (m_ret != SQL_SUCCESS) {
		cout << "Ошибка подключения к MySQL" << endl;
	}
	m_ret = SQLExecDirectA(sqlStmt, (SQLCHAR*)"SET NAMES utf8mb4", SQL_NTS);
	m_ret = SQLExecDirectA(sqlStmt, (SQLCHAR*)"SET CHARACTER SET utf8mb4", SQL_NTS);
	initdb();
	cout << "--- ODBC ---" << endl;
}

void DB_MyODBC::sendRequest(const string& request) {
	m_query = request;
	m_ret = SQLAllocHandle(SQL_HANDLE_STMT, sqlConn, &sqlStmt);
	if (m_ret != SQL_SUCCESS) {
		cout << "Ошибка создания оператора" << endl;
	}
	if (SQL_SUCCESS != SQLExecDirectA(sqlStmt, (SQLCHAR*)m_query.c_str(), SQL_NTS)) {
		cout << "Запрос " + m_query + " выполнен c ошибкой." << endl;
	}
	m_query.clear();
}

vector<string> DB_MyODBC::getRequest(const string& request, int& num_rows, int& num_fields) {
	vector<string> result;
	m_query = request;
	SQLINTEGER row = 0;
	SQLSMALLINT field = 0;
	SQLLEN indicator;
	SQLCHAR data[1024];

	// Подготовка и выполнение запроса
	m_ret = SQLAllocHandle(SQL_HANDLE_STMT, sqlConn, &sqlStmt);
	if (m_ret != SQL_SUCCESS) {
		cout << "Ошибка создания оператора" << endl;
	}
	m_ret = SQLExecDirectA(sqlStmt, (SQLCHAR*)m_query.data(), SQL_NTS);
	if (m_ret != SQL_SUCCESS) {
		cout << "Запрос " + m_query + " выполнен c ошибкой." << endl;
	}
	// Получение метаданных результата
	m_ret = SQLNumResultCols(sqlStmt, &m_fields);
	if (m_ret != SQL_SUCCESS) {
		cout << "Ошибка получения количества полей" << endl;
	}
	// Получение количества строк
	m_ret = SQLRowCount(sqlStmt, &m_rows);
	if (m_ret != SQL_SUCCESS) {
		cout << "Ошибка получения количества строк" << endl;
	}
	// Получение данных
	while ((m_ret = SQLFetch(sqlStmt)) == SQL_SUCCESS) {
		for (field = 1; field <= m_fields; ++field) {
			m_ret = SQLGetData(sqlStmt, field, SQL_C_CHAR, data, sizeof(data), &indicator);
			if (m_ret == SQL_SUCCESS) {
				result.push_back(string((char*)data));
			}
		}
	}
	num_rows = m_rows;
	num_fields = m_fields;
	// Освобождение ресурсов
	SQLFreeHandle(SQL_HANDLE_STMT, sqlStmt);
	m_query.clear();
	return result;
}

void DB_MyODBC::initdb() {
	m_query = "CREATE DATABASE IF NOT EXISTS " + m_db_name;
	sendRequest(m_query);
	m_query = "USE " + m_db_name;
	sendRequest(m_query);
	m_query = "CREATE TABLE IF NOT EXISTS users ("
		"id SERIAL PRIMARY KEY,"
		"login VARCHAR(50) NOT NULL UNIQUE,"
		"password char(10) NOT NULL DEFAULT '0',"
		"name VARCHAR(100) NOT NULL,"
		"status TINYINT NOT NULL DEFAULT 0 CHECK(status IN(0, -1, 1))"
		")";
	sendRequest(m_query);
	m_query = "CREATE TABLE IF NOT EXISTS messages ("
		"id SERIAL PRIMARY KEY,"
		"msg_text TEXT NOT NULL,"
		"msg_from INTEGER NOT NULL REFERENCES users(id),"
		"msg_to INTEGER NOT NULL DEFAULT 0,"
		"msg_status TINYINT NOT NULL DEFAULT 0 CHECK(msg_status IN(0, 1, 2)),"
		"msg_timestamp INTEGER UNSIGNED NOT NULL DEFAULT 0"
		")";
	sendRequest(m_query);
	m_query = "CREATE TABLE IF NOT EXISTS pwdhashes ("
		"user_id INTEGER NOT NULL REFERENCES users(id),"
		"pwdhash VARCHAR(10) NOT NULL"
		")";
	sendRequest(m_query);
}
#endif