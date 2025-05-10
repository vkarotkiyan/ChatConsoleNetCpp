#include "misc.h"
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
