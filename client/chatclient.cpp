#include <filesystem>
#include <fstream>
#include "chatclient.h"
#ifdef max
#undef max
#endif
using namespace std;

ChatClient::ChatClient(string ip, int port) { 
	net = new NetClient(ip, port);
	net->start();
	m_isWork = true; 
}

ChatClient::~ChatClient() {
	delete net;
}

bool ChatClient::isWork() const { return m_isWork; }

void ChatClient::initMenu() {
	string codeMsg;
	char oper = '0';
	do {
		if (!m_currentUser) {
			cout << "Регистрация (1) | Вход в чат (2) | Завершение работы (0)" << endl;
			cin >> oper;
		}
		switch (oper) {
		case '1':
			try {
				signUp();
			}
			catch (const exception& e) {
				cout << e.what() << endl;
			}
			break;
		case '2':
			login();
			userMenu();
			break;
		case '0':
			codeMsg = to_string(RequestAPI::ServerExit);
			net->sendMsg(codeMsg);
			net->Exit();
			clear_screen();
			cout << "Сервер завершил работу." << endl;
			m_isWork = false;
			break;
		default:
			clear_screen();
			cout << "Ошибка ввода. Введите 1, 2 или 0" << endl;
			break;
		}
	} while (m_isWork);
}

void ChatClient::login() {
	string login, password;
	char oper;
	string code = to_string(RequestAPI::Login);
	clear_screen();
	do {
		cout << "Логин: ";
		cin >> login;
		cout << "Пароль: ";
		cin >> password;
		net->sendMsg(code);
		net->sendMsg(login);
		net->sendMsg(password);
		if (net->getMsg() == "true")
			m_currentUser = getUserByLogin(login);
		else {
			m_currentUser = nullptr;
			cout << "Ошибка входа." << endl;
			cout << "Нажмите '0' для выхода в меню или любую клавишу для повтора: ";
			cin >> oper;
			if (oper == '0')
				break;
		}
	} while (!m_currentUser);
	clear_screen();
}

void ChatClient::signUp() { // Регистрация пользователя
	string login, password, name;
	string code = to_string(RequestAPI::Registration);
	clear_screen();
	cout << "Логин: ";
	cin >> login;
	cout << "Пароль: ";
	cin >> password;
	cout << "Имя: ";
	cin >> name;
	cp1251toUtf8(name);
	net->sendMsg(code);
	net->sendMsg(login);
	net->sendMsg(password);
	net->sendMsg(name);
	if (net->getMsg() == "success") {
		re_readChat();
		clear_screen();
		cout << "Регистрация прошла успешно." << endl;
	} else {
		if (getUserByLogin(login) || login == "all") {
			throw UserLoginExc();
		}
		if (getUserByName(name) || name == "all") {
			throw UserNameExc();
		}
	}
}

void ChatClient::showUserList() const {
	cout << "----------------- Список пользователей -----------------" << endl;
	for (auto& user : m_users) {
		cout << user->getUserName() << " (" << user->getUserLogin() << ")";
		if (m_currentUser)
			if (m_currentUser->getUserLogin() == user->getUserLogin())
				cout << " (активный пользователь)";
		cout << endl;
	}
	cout << "--------------------------------------------------------" << std::endl;
}

void ChatClient::userMenu() {
	char oper;
	string codeMsg;
	while (m_currentUser) {
		showUserList();
		if (!m_userForChat)
			cout << "Выбрать пользователя для общения (1)";
		else
			cout << "В общий чат (1) | Поменять собеседника (3)";
		cout << " | Добавить сообщение (2) | Выход из чата (0)" << endl;
		re_readChat();
		showChat();
		cin >> oper;
		switch (oper) {
		case '1':
			clear_screen();
			if (!m_userForChat) setUserForChat();
			else m_userForChat = nullptr;
			break;
		case '2':
			clear_screen();
			showUserList();
			if (!m_userForChat)
				cout << "Выбрать пользователя для общения (1)";
			else
				cout << "Перейти в общий чат (1) | Сменить собеседника (3)";
			cout << " | Добавить сообщение (2) | Выход из чата (0)" << endl;
			re_readChat();
			showChat();
			addMessage();
			break;
		case '3':
			setUserForChat();
			break;
		case '0':
			clear_screen();
			m_userForChat = nullptr;
			m_currentUser = nullptr;
			break;
		default:
			clear_screen();
			cout << "Ошибка ввода. Введите 1, 2, 3 или 0" << endl;
			break;
		}
	}
}

shared_ptr<User> ChatClient::getUserByLogin(const string& login) const {
	for (auto& user : m_users) {
		if (login == user->getUserLogin()) return user; // Исправлено по рекомендации проверяющего
	}
	return nullptr;
}

shared_ptr<User> ChatClient::getUserByName(const string& name) const {
	for (auto& user : m_users) {
		if (name == user->getUserName()) return user; // Исправлено по рекомендации проверяющего
	}
	return nullptr;
}

shared_ptr<User> ChatClient::getCurrentUser() const {
	return m_currentUser;
}

shared_ptr<User> ChatClient::getUserForChat() const {
	return m_userForChat;
}

void ChatClient::setUserForChat() {
	string to;
	clear_screen();
	cout << "Введите имя собеседника: ";
	cin >> to;
	cp1251toUtf8(to);
	if (!getUserByName(to)) {
		cout << "Ошибка: пользователя с именем " << to << " не существует." << endl;
		return;
	}
	else m_userForChat = getUserByName(to);
}

void ChatClient::addMessage() {
	string text;
	string from = m_currentUser->getUserLogin();
	string to = (m_userForChat) ? m_userForChat->getUserLogin() : "all";
	string code = to_string(RequestAPI::AddMessage);
	cout << ">> ";
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	getline(cin, text);
	cp1251toUtf8(text);
	net->sendMsg(code);
	net->sendMsg(text);
	net->sendMsg(from);
	net->sendMsg(to);
	if (net->getMsg() == "sucess") {
		re_readChat();
	}
	clear_screen();
}

void ChatClient::showChat() const { // Вывод списка сообщений. Сообщения активного пользователя выводятся с отступом.
	if (!m_userForChat)
		cout << "----------------------------- Общий чат -----------------------------------" << endl;
	else
		cout << "------------------------- Чат с пользователем " << m_userForChat->getUserName() << " ------------------------" << endl;
	for (auto& mess : m_messages) {
		if (!m_userForChat) {
			if (mess.getTo() == "all") {
				if (mess.getFrom() == m_currentUser->getUserLogin()) cout << "          ";
				cout << showTime(mess.getTime()) << " " << getUserByLogin(mess.getFrom())->getUserName()
					<< ": " << mess.getText() << endl;
			}
		}
		else {
			if ((mess.getFrom() == m_currentUser->getUserLogin() && mess.getTo() == m_userForChat->getUserLogin()) ||
				(mess.getFrom() == m_userForChat->getUserLogin() && mess.getTo() == m_currentUser->getUserLogin()))
			{
				if (m_currentUser->getUserLogin() == mess.getFrom()) cout << "          ";
				cout << showTime(mess.getTime()) << " " << getUserByLogin(mess.getFrom())->getUserName()
					<< ": " << mess.getText() << endl;
			}
		}
	}
	cout << "---------------------------------------------------------------------------" << endl;
}

void ChatClient::readChat() { // Чтение двнных чата из файлов
	fstream user_file_r = fstream(pathFile(U_FILE), ios::in); // Заполняем пользователей
	if (user_file_r.is_open()) {
		const string delimiter = "::";
		string str, login, password, name;
		while (getline(user_file_r, str)) {
			size_t end = str.find(delimiter);
			login = str.substr(0, end);
			str.erase(str.begin(), str.begin() + end + 2);
			end = str.find(delimiter);
			password = str.substr(0, end);
			name = str.substr(end + 2);
			shared_ptr<User> user(new User(login, password, name));
			m_users.push_back(user);
		}
	}
	user_file_r.close();

	fstream message_file_r = fstream(pathFile(M_FILE), ios::in); // Заполняем сообщения
	if (message_file_r.is_open()) {
		const string delimiter = "::";
		string str, from, to, text;
		time_t time;
		while (getline(message_file_r, str)) {
			size_t end = str.find(delimiter);
			time = stoll(str.substr(0, end));
			str.erase(str.begin(), str.begin() + end + 2);
			end = str.find(delimiter);
			from = str.substr(0, end);
			str.erase(str.begin(), str.begin() + end + 2);
			end = str.find(delimiter);
			to = str.substr(0, end);
			text = str.substr(end + 2);
			m_messages.push_back(Message{ text, from, to, time });
		}
	}
	message_file_r.close();
}

void ChatClient::re_readChat() {
	fstream user_file_r = fstream(pathFile(U_FILE), ios::in); // Заполняем пользователей
	if (user_file_r.is_open()) {
		const string delimiter = "::";
		string str, login, password, name;
		m_users.clear();
		while (getline(user_file_r, str)) {
			size_t end = str.find(delimiter);
			login = str.substr(0, end);
			str.erase(str.begin(), str.begin() + end + 2);
			end = str.find(delimiter);
			password = str.substr(0, end);
			name = str.substr(end + 2);
			shared_ptr<User> user(new User(login, password, name));
			m_users.push_back(user);
		}
	}
	user_file_r.close();

	fstream message_file_r = fstream(pathFile(M_FILE), ios::in); // Заполняем сообщения
	if (message_file_r.is_open()) {
		const string delimiter = "::";
		string str, from, to, text;
		time_t time;
		m_messages.clear();
		while (getline(message_file_r, str)) {
			size_t end = str.find(delimiter);
			time = stoll(str.substr(0, end));
			str.erase(str.begin(), str.begin() + end + 2);
			end = str.find(delimiter);
			from = str.substr(0, end);
			str.erase(str.begin(), str.begin() + end + 2);
			end = str.find(delimiter);
			to = str.substr(0, end);
			text = str.substr(end + 2);
			m_messages.push_back(Message{ text, from, to, time });
		}
	}
	message_file_r.close();
}

void ChatClient::ServerExit() {
	string codeMsg = to_string(RequestAPI::ServerExit);
		net->sendMsg(codeMsg);
}
