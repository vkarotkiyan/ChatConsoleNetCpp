#pragma once
#include <iostream>
#include <exception>
#include <cstdlib>
#include <string>
#include <locale>
#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#elif defined(__linux__)
#include <unistd.h>
#endif
constexpr auto U_FILE = "users.txt";
constexpr auto M_FILE = "messages.txt";

enum RequestAPI {
	Registration,
	Login,
	AddMessage,
	ClientExit,
	ServerExit
};

// Настройка консоли для Windows и Linux
void set_cons();

// Очистка экрана консоли
void clear_screen();

// Вычисление домашнего каталога + имя файла
std::string pathFile(const char* file);

// Информация об ОС и процессе
void info();

// Отображение времени в правильном формате
std::string showTime(const time_t&);

// Перекодировка строки для Windows
void cp1251toUtf8(std::string&);

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
