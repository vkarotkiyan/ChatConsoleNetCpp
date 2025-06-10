#include "db_mysql.h"
using namespace  std;

DB_MySQL::~DB_MySQL() {
	mysql_close(&mysql);
	cout << "Соединение с базой данных закрыто." << endl;
}

void DB_MySQL::db_start() {
	mysql_init(&mysql);
	if (&mysql == NULL)	{
		cout << "ERROR: Ошибка создания MySQL дескриптора" << endl;
		exit(1);
	}
	if (!mysql_real_connect(&mysql, m_ip.data(), m_db_login.data(), m_db_pass.data(), NULL, m_port, NULL, 0)) {
		cout << "ERROR: Ошибка подключения к базе данных " + string(mysql_error(&mysql));
		exit(1);
	}
	mysql_set_character_set(&mysql, "utf8");
	initdb();
	cout << "--- MySQLAPI ---" << endl;
}

void DB_MySQL::sendRequest(const string& request) {
	m_query = request;
	int query = mysql_query(&mysql, m_query.data());
	if (query != 0) {
		cout << "Запрос " + request + " выполнен c ошибкой: " + std::to_string(query) << endl;
	}
	m_query.clear();
}

vector<string> DB_MySQL::getRequest(const string& request, int& num_rows, int& num_fields) {
	vector<string> result;
	m_query = request;
	mysql_query(&mysql, m_query.data());
	if ((m_result = mysql_store_result(&mysql))) {
		while (m_row = mysql_fetch_row(m_result))
			for (int i = 0; i < mysql_num_fields(m_result); i++) result.push_back(m_row[i]);
	}
	else 
		cout << "Ошибка MySQL " + string(mysql_error(&mysql));
	num_rows = mysql_num_rows(m_result);
	num_fields = mysql_num_fields(m_result);
	m_query.clear();
	return result;
}

void DB_MySQL::initdb() {
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
