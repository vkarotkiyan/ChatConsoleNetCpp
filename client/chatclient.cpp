#include "chatclient.h"
#ifdef max
#undef max
#endif
using namespace std;

ChatClient::ChatClient() {
	vector<string> config = readConfig();
	net = new NetClient(config[0], stoi(config[1]));
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
		string passHash = to_string(*sha1(password.c_str(), password.length()));
		net->sendMsg(code);
		net->sendMsg(login);
		net->sendMsg(passHash);
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
	cp1251toUtf8(login);
	while (!сhесkUserInput(login)) {
		cin >> login;
		cp1251toUtf8(login);
	}
	cout << "Пароль: ";
	cin >> password;
	cp1251toUtf8(password);
	while (!сhесkUserInput(password)) {
		cin >> password;
		cp1251toUtf8(password);
	}
	cout << "Имя: ";
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	getline(cin, name);
	cp1251toUtf8(name);
	string passHash = to_string(*sha1(password.c_str(), password.length()));
	net->sendMsg(code);
	net->sendMsg(login);
	net->sendMsg(passHash);
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

std::shared_ptr<User> ChatClient::getUserById(const int& id) const {
	for (auto& user : m_users) {
		if (id == user->getUserId()) return user; 
	}
	return nullptr;
}

shared_ptr<User> ChatClient::getUserByLogin(const string& login) const {
	for (auto& user : m_users) {
		if (login == user->getUserLogin()) return user; 
	}
	return nullptr;
}

shared_ptr<User> ChatClient::getUserByName(const string& name) const {
	for (auto& user : m_users) {
		if (name == user->getUserName()) return user; 
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
	string from = to_string(m_currentUser->getUserId());
	string to = (m_userForChat) ? to_string(m_userForChat->getUserId()) : "0";
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
			if (mess.getTo() == 0) {
				if (mess.getFrom() == m_currentUser->getUserId()) cout << "          ";
				cout << showTime(mess.getTime()) << " " << getUserById(mess.getFrom())->getUserName()
					<< ": " << mess.getText() << endl;
			}
		}
		else {
			if ((mess.getFrom() == m_currentUser->getUserId() && mess.getTo() == m_userForChat->getUserId()) ||
				(mess.getFrom() == m_userForChat->getUserId() && mess.getTo() == m_currentUser->getUserId()))
			{
				if (m_currentUser->getUserId() == mess.getFrom()) cout << "          ";
				cout << showTime(mess.getTime()) << " " << getUserById(mess.getFrom())->getUserName()
					<< ": " << mess.getText() << endl;
			}
		}
	}
	cout << "---------------------------------------------------------------------------" << endl;
}

void ChatClient::readChat() { // Чтение данных чата из базы
	int id, status, from, to;
	string text, login, password, name;
	time_t time;
	int num_rows = 0;
	string code = to_string(RequestAPI::ReadChat);
	net->sendMsg(code);
	num_rows = stoi(net->getMsg());
	for (int i = 0; i < num_rows; i++) {
		id = stoi(net->getMsg());
		login = net->getMsg();
		password = net->getMsg();
		name = net->getMsg();
		status = stoi(net->getMsg());
		shared_ptr<User> user(new User(id, login, password, name, status));
		m_users.push_back(user);
	}
	num_rows = stoi(net->getMsg());
	for (int i = 0; i < num_rows; i++) {
		id = stoi(net->getMsg());
		text = net->getMsg();
		from = stoi(net->getMsg());
		to = stoi(net->getMsg());
		status = stoi(net->getMsg());
		time = stoll(net->getMsg());
		m_messages.push_back(Message{ id, text, from, to, status, time });
	}
}

void ChatClient::re_readChat() {
	int id, status, from, to;
	string text, login, password, name;
	time_t time;
	int num_rows = 0;
	m_users.clear();
	m_messages.clear();
	string code = to_string(RequestAPI::ReadChat);
	net->sendMsg(code);
	num_rows = stoi(net->getMsg());
	for (int i = 0; i < num_rows; i++) {
		id = stoi(net->getMsg());
		login = net->getMsg();
		password = net->getMsg();
		name = net->getMsg();
		status = stoi(net->getMsg());
		shared_ptr<User> user(new User(id, login, password, name, status));
		m_users.push_back(user);
	}
	num_rows = stoi(net->getMsg());
	for (int i = 0; i < num_rows; i++) {
		id = stoi(net->getMsg());
		text = net->getMsg();
		from = stoi(net->getMsg());
		to = stoi(net->getMsg());
		status = stoi(net->getMsg());
		time = stoll(net->getMsg());
		m_messages.push_back(Message{ id, text, from, to, status, time });
	}
}

void ChatClient::ServerExit() {
	string codeMsg = to_string(RequestAPI::ServerExit);
		net->sendMsg(codeMsg);
}
