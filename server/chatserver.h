#pragma once
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <list>
#include "netserver.h"
#include "user.h"
#include "message.h"
#include "misc.h"
#include "db.h"
#include "db_Interface.h"

class ChatServer {
public:
	ChatServer();
	~ChatServer();
	bool isWork() const;
	void initChat();
#if defined (_WIN64) || defined(_WIN32)
	//void clientHandler(SOCKET newSocket, std::list<SOCKET>::iterator it);
	void clientHandler(SOCKET newSocket);
	void login(SOCKET newSock); // Аутентификация пользователя
	//void CloseClient(std::list<SOCKET>::iterator it);
	void CloseClient(SOCKET newSocket);
	void signUp(SOCKET newSock); // Регистрация пользователя
	void addMessage(SOCKET newSocket); // Ввод сообщения
	void readChatFromDB(SOCKET newSocket);
#elif defined (__linux__)
	void clientHandler(int newConnect);
	void login(int newConnect); // Аутентификация пользователя
	void CloseClient(int newConnect);
	void signUp(int newConnect); // Регистрация пользователя
	void addMessage(int newConnect); // Ввод сообщения
	void readChatFromDB(int newConnect);
#endif
	void showUserList() const; // Отображение списка пользователей
	std::shared_ptr<User> getUserById(const int&) const;
	std::shared_ptr<User> getUserByLogin(const std::string&) const;
	std::shared_ptr<User> getUserByName(const std::string&) const;
	//std::shared_ptr<User> getCurrentUser() const;
	//std::shared_ptr<User> getUserForChat() const;
	//void writeChat();
private:
	bool m_isWork = false;
	NetServer *net;
	DB* db;
	//std::list<SOCKET> connect_list;
	std::vector<std::shared_ptr<User>> m_users; // Массив пользователей
	std::vector<Message> m_messages; // Массив сообщений
	std::shared_ptr<User> m_currentUser = nullptr; // Активный пользователь
	std::shared_ptr<User> m_userForChat = nullptr; // Пользователь, с которым ведется общение
};
