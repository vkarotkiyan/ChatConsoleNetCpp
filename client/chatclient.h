#pragma once
#include <vector>
#include <memory>
#include "user.h"
#include "message.h"
#include "net.h"
#include "misc.h"
#include "sha1.h"

class ChatClient {
public:
	ChatClient();
	~ChatClient();
	bool isWork() const;
	void initMenu(); // Начальное меню
	void login(); // Аутентификация пользователя
	void signUp(); // Регистрация пользователя
	void showUserList() const; // Отображение списка пользователей
	void userMenu(); // Меню авторизованного пользователя
	std::shared_ptr<User> getUserById(const int&) const;
	std::shared_ptr<User> getUserByLogin(const std::string&) const;
	std::shared_ptr<User> getUserByName(const std::string&) const;
	std::shared_ptr<User> getCurrentUser() const;
	std::shared_ptr<User> getUserForChat() const;
	void setUserForChat(); // Выбор пользователя для общения
	void addMessage(); // Ввод сообщения
	void showChat() const; // Отображение списка сообщений
	void readChat();
	void re_readChat();
	void ServerExit();
private:
	bool m_isWork = false;
	NetClient *net;
	std::vector<std::shared_ptr<User>> m_users; // Массив пользователей
	std::vector<Message> m_messages; // Массив сообщений
	std::shared_ptr<User> m_currentUser = nullptr; // Активный пользователь
	std::shared_ptr<User> m_userForChat = nullptr; // Пользователь, с которым ведется общение
};
