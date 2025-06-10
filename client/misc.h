#pragma once
#include <iostream>
#include <exception>
#include <cstdlib>
#include <string>
#include <vector>
#include <locale>
#include <filesystem>
#include <fstream>
#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

using std::string;

constexpr auto C_FILE = "config_client.ini";

enum RequestAPI {
	ReadChat,
	Registration,
	Login,
	AddMessage,
	ClientExit,
	ServerExit
};

void set_cons(); // Настройка консоли для Windows и Linux

void clear_screen(); // Очистка экрана консоли

std::string pathFile(const char* file); // Вычисление домашнего каталога + имя файла

void info(); // Информация об ОС и процессе

std::string showTime(const time_t&); // Отображение времени в правильном формате

void cp1251toUtf8(std::string&); // Перекодировка строки для Windows

bool сhесkUserInput(std::string& str); // Проверка пользовательского ввода

std::vector<std::string> readConfig(); // Чтение и сохранение конфига
void writeConfig();
std::string readConfigValue(std::string value);

// Exceptions /////////////////////////////////////////////////////////////////

class UserLoginExc : public std::exception {
public:
	virtual const char* what() const noexcept override {
		clear_screen();
		return "Ошибка: такой логин уже существует.\n";
	};
};

class UserNameExc : public std::exception {
public:
	virtual const char* what() const noexcept override {
		clear_screen();
		return "Ошибка: такое имя уже существует.\n";
	};
};
