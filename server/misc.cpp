#include "misc.h"

namespace fs = std::filesystem;
using namespace  std;

void set_cons() {
#if defined(_WIN64) || defined(_WIN32)
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(1251);
#elif defined(__linux__)
	setlocale(LC_ALL, "");
#endif
}

void clear_screen() {
#if defined(_WIN64) || defined(_WIN32)
	system("cls");
#elif defined(__linux__)
	system("clear");
#endif
}

string pathFile(const char* file) {
#if defined(_WIN64) || defined(_WIN32)
	string str1 = getenv("HOMEDRIVE");
	string str2 = getenv("HOMEPATH");
	return str1 + str2 + "/" + (string)file;
#elif defined(__linux__)
	return getenv("HOME") + (string)"/" + (string)file;
#endif
}

void info() {
#if defined(_WIN64) || defined(_WIN32)
	cout << "Windows ";
	cout << "PID = " << GetCurrentProcessId() << " Сервер чата." << endl;
#elif defined(__linux__)
	cout << "Linux ";
	cout << "PID = " << getpid() << " Сервер чата." << endl;
#endif
}

string showTime(const time_t& time) {
	struct tm* timeinfo;
	char buffer[80];
	timeinfo = localtime(&time);
	strftime(buffer, 80, "%d.%m.%Y %H:%M", timeinfo);
	return string(buffer);
}

void cp1251toUtf8(string& str) {
#if defined(_WIN32) || defined(_WIN64)
	int result_u, result_c;
	result_u = MultiByteToWideChar(1251, 0, str.data(), -1, 0, 0);
	if (!result_u)
		return;
	wchar_t* ures = new wchar_t[result_u];
	if (!MultiByteToWideChar(1251, 0, str.data(), -1, ures, result_u)) {
		delete[] ures;
		return;
	}
	result_c = WideCharToMultiByte(65001, 0, ures, -1, 0, 0, 0, 0);
	if (!result_c) {
		delete[] ures;
		return;
	}
	char* cres = new char[result_c];
	if (!WideCharToMultiByte(65001, 0, ures, -1, cres, result_c, 0, 0)) {
		delete[] cres;
		return;
	}
	str = cres;
	delete[] ures, cres;
#endif
}

vector<string> readConfig() {
	fstream config_file_r = fstream(pathFile(C_FILE), ios::in);
	string str;
	vector<string> config;
	if (config_file_r.is_open()) {
		const string delimiter = " = ";
		while (getline(config_file_r, str)) {
			if (str.starts_with("server_ip"))
				config.push_back(str.erase(0, str.find(delimiter) + 3));
			if (str.starts_with("server_port"))
				config.push_back(str.erase(0, str.find(delimiter) + 3));
			if (str.starts_with("mysql_server_ip"))
				config.push_back(str.erase(0, str.find(delimiter) + 3));
			if (str.starts_with("mysql_server_port"))
				config.push_back(str.erase(0, str.find(delimiter) + 3));
			if (str.starts_with("mysql_database"))
				config.push_back(str.erase(0, str.find(delimiter) + 3));
			if (str.starts_with("mysql_login"))
				config.push_back(str.erase(0, str.find(delimiter) + 3));
			if (str.starts_with("mysql_password"))
				config.push_back(str.erase(0, str.find(delimiter) + 3));
			if (str.starts_with("db_interface"))
				config.push_back(str.erase(0, str.find(delimiter) + 3));
		}
		config_file_r.close();
	}
	else {
		config.push_back("127.0.0.1");
		config.push_back("7777");
		config.push_back("localhost");
		config.push_back("3306");
		config.push_back("chatdb");
		config.push_back("root");
		config.push_back("root");
		config.push_back("mysqlapi");
		writeConfig();
	}
	return config;
}

void writeConfig() {
	string ip = "127.0.0.1";
	string port = "7777";
	string mysql_server_ip = "localhost";
	string mysql_server_port = "3306";
	string mysql_database = "chatdb";
	string mysql_login = "root";
	string mysql_password = "root";
	string db_interface = "mysqlapi";
	fstream config_file_wr = fstream(pathFile(C_FILE), ios::out);
	if (!config_file_wr.is_open()) {
		cout << "ERROR: Ошибка открытия файла!" << endl;
	}
	else {
		fs::permissions(pathFile(C_FILE), fs::perms::group_all | fs::perms::others_all, fs::perm_options::remove);
		config_file_wr << "server_ip = " << ip << "\n";
		config_file_wr << "server_port = " << port << "\n";
		config_file_wr << "mysql_server_ip = " << mysql_server_ip << "\n";
		config_file_wr << "mysql_server_port = " << mysql_server_port << "\n";
		config_file_wr << "mysql_database = " << mysql_database << "\n";
		config_file_wr << "mysql_login = " << mysql_login << "\n";
		config_file_wr << "mysql_password = " << mysql_password << "\n";
		config_file_wr << "db_interface = " << db_interface << "\n";
		config_file_wr.close();
	}
}

std::string readConfigValue(std::string value) {
	const string delimiter = " = ";
	string str, confValue;
	fstream config_file_r = fstream(pathFile(C_FILE), ios::in);
	if (config_file_r.is_open()) {
		while (getline(config_file_r, str))
			if (str.starts_with(value))
				confValue = str.erase(0, str.find(delimiter) + 3);
		config_file_r.close();
	}
	return confValue;
}
