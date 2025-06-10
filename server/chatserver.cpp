#include <iostream>
#include <filesystem>
#include <fstream>
#include "chatserver.h"

namespace fs = std::filesystem;
using namespace std;

ChatServer::ChatServer() { 
	vector<string> config = readConfig();
	net = new NetServer(config[0], stoi(config[1]));
	net->start();
	db = (new DB_Interface)->DBInitInterface(config[2], stoi(config[3]), config[4], config[5], config[6], config[7]);
	db->db_start();
	m_isWork = true;
}

ChatServer::~ChatServer() {
	delete db;
	delete net;
}

bool ChatServer::isWork() const { return m_isWork; }

void ChatServer::initChat() {
#if defined (_WIN64) || defined(_WIN32)
	while (m_isWork) {
		SOCKET newSocket = net->acception();
		/*
				connect_list.push_back(newSocket);
				list<SOCKET>::iterator it = connect_list.end();
				--it;
				thread th(clientHandler(newSocket, it));
		*/
		clientHandler(newSocket);
	};
#elif defined (__linux__)
	while (m_isWork) {
		net->acception();
		clientHandler(net->getConnect());
	};
#endif
}

#if defined (_WIN64) || defined(_WIN32)

//void ChatServer::clientHandler(SOCKET newSocket, std::list<SOCKET>::iterator it) {
void ChatServer::clientHandler(SOCKET newSocket) {
	string codeMsg = "";
	while (codeMsg != to_string(RequestAPI::ServerExit)) {
		codeMsg = net->getMsg(newSocket);
		int oper = stoi(codeMsg);
		switch (oper) {
		case RequestAPI::ReadChat:
			readChatFromDB(newSocket);
			break;
		case RequestAPI::Registration:
			signUp(newSocket);
			break;
		case RequestAPI::Login:
			login(newSocket);
			break;
		case RequestAPI::AddMessage:
			addMessage(newSocket);
			break;
		case RequestAPI::ClientExit:
			CloseClient(newSocket);
			m_currentUser = nullptr;
			cout << "Клиент завершил работу." << endl;
			break;
		case RequestAPI::ServerExit:
			net->Exit();
			m_currentUser = nullptr;
			m_isWork = false;
			break;
		default:
			break;
		}
	}
}

void ChatServer::login(SOCKET newSock) {
	string login = net->getMsg(newSock);
	string passHash = net->getMsg(newSock);
	m_currentUser = getUserByLogin(login);
	if (m_currentUser == nullptr) {
		net->sendMsg("false", newSock);
		return;
	}
	else {
		int num_rows = 0, num_fields = 0;
		string getUserFromDB = "SELECT u.id, u.login, p.pwdhash, u.name, u.status FROM users u JOIN pwdhashes p ON p.user_id = u.id WHERE u.login = '" + 
			login + "';";
		vector<string> userDB = db->getRequest(getUserFromDB, num_rows, num_fields);
		if (userDB[2] == passHash) {
			shared_ptr<User> user(new User(stoi(userDB[0]), userDB[1], userDB[2], userDB[3], stoi(userDB[4])));
			m_users.push_back(user);
		} else {
			net->sendMsg("false", newSock);
			return;
		}
	}
	net->sendMsg("true", newSock);
	cout << "Пользователь с логином " << m_currentUser->getUserLogin() << " вошел в чат." << endl;
}

/*
void ChatServer::CloseClient(list<SOCKET>::iterator it) {
	closesocket(*it);
	//connect_list.erase(it);
}
*/

void ChatServer::CloseClient(SOCKET newSocket) {
	closesocket(newSocket);
}

void ChatServer::signUp(SOCKET newSocket) {
	string login = net->getMsg(newSocket);
	string passHash = net->getMsg(newSocket);
	string name = net->getMsg(newSocket);
	if (getUserByLogin(login) || login == "all" || getUserByName(name) || name == "all") {
		net->sendMsg("login_not_uniq", newSocket);
	}
	else {
		string addUserToDB1 = "INSERT INTO users(login, name, password) VALUES ('" + login + "', '" + name + "', '0');";
		string addUserToDB2 = "INSERT INTO pwdhashes (user_id, pwdhash) SELECT id, '" + passHash + "' FROM users " +
			"WHERE login = '" + login + "';";
		db->sendRequest(addUserToDB1);
		db->sendRequest(addUserToDB2);
		net->sendMsg("success", newSocket);
		cout << "Пользователь с логином " + login + " зарегистрировался." << endl;
	}
}

void ChatServer::addMessage(SOCKET newSocket) {
	string text = net->getMsg(newSocket);
	string from = net->getMsg(newSocket);
	string to = net->getMsg(newSocket);
	string addMessageToDB = "INSERT INTO messages(msg_text, msg_from, msg_to, msg_timestamp) VALUES('" +
		text + "', '" + from + "', '" + to + "', UNIX_TIMESTAMP(now()));";
	db->sendRequest(addMessageToDB);
	net->sendMsg("success", newSocket);
	cout << "Сообщение от пользователя с логином " << getUserById(stoi(from))->getUserLogin() << " добавлено." << endl;
}

void ChatServer::readChatFromDB(SOCKET newSocket) {
	vector<string> usersTable, messagesTable;
	int num_rows = 0, num_fields = 0;
	string getUsersfromDB = "SELECT * FROM users ORDER BY id;";
	usersTable = db->getRequest(getUsersfromDB, num_rows, num_fields);
	net->sendMsg(to_string(num_rows), newSocket);
	int j = 0;
	for (int i = 0; i < num_rows; i++) {
		net->sendMsg(usersTable[j], newSocket);
		net->sendMsg(usersTable[j + 1], newSocket);
		net->sendMsg(usersTable[j + 2], newSocket);
		net->sendMsg(usersTable[j + 3], newSocket);
		net->sendMsg(usersTable[j + 4], newSocket);
		shared_ptr<User> user(new User(stoi(usersTable[j]), usersTable[j + 1], usersTable[j + 2], usersTable[j + 3], stoi(usersTable[j + 4])));
		m_users.push_back(user);
		j += num_fields;
	}
	string getMessagesfromBase = "SELECT * FROM messages ORDER BY id;";
	messagesTable = db->getRequest(getMessagesfromBase, num_rows, num_fields);
	net->sendMsg(to_string(num_rows), newSocket);
	j = 0;
	for (int i = 0; i < num_rows; i++) {
		net->sendMsg(messagesTable[j], newSocket);
		net->sendMsg(messagesTable[j + 1], newSocket);
		net->sendMsg(messagesTable[j + 2], newSocket);
		net->sendMsg(messagesTable[j + 3], newSocket);
		net->sendMsg(messagesTable[j + 4], newSocket);
		net->sendMsg(messagesTable[j + 5], newSocket);
		j += num_fields;
	}
}

#elif defined (__linux__)

void ChatServer::clientHandler(int newConnect) {
	string codeMsg = "";
	while (codeMsg != to_string(RequestAPI::ServerExit)) {
		codeMsg = net->getMsg(newConnect);
		int oper = stoi(codeMsg);
		switch (oper) {
		case RequestAPI::ReadChat:
			readChatFromDB(newConnect);
			break;
		case RequestAPI::Registration:
			signUp(newConnect);
			break;
		case RequestAPI::Login:
			login(newConnect);
			break;
		case RequestAPI::AddMessage:
			addMessage(newConnect);
			break;
		case RequestAPI::ClientExit:
			CloseClient(newConnect);
			m_currentUser = nullptr;
			cout << "Клиент завершил работу." << endl;
			break;
		case RequestAPI::ServerExit:
			net->Exit();
			m_currentUser = nullptr;
			m_isWork = false;
			break;
		default:
			break;
		}
	}
}

void ChatServer::login(int newConnect) {
	string login = net->getMsg(newConnect);
	string passHash = net->getMsg(newConnect);
	m_currentUser = getUserByLogin(login);
	if (m_currentUser == nullptr) {
		net->sendMsg("false", newConnect);
		return;
	} else {
		int num_rows = 0, num_fields = 0;
		string getUserFromDB = "SELECT u.id, u.login, p.pwdhash, u.name, u.status FROM users u JOIN pwdhashes p ON p.user_id = u.id WHERE u.login = '" +
			login + "';";
		vector<string> userDB = db->getRequest(getUserFromDB, num_rows, num_fields);
		if (userDB[2] == passHash) {
			shared_ptr<User> user(new User(stoi(userDB[0]), userDB[1], userDB[2], userDB[3], stoi(userDB[4])));
			m_users.push_back(user);
		} else {
			net->sendMsg("false", newConnect);
			return;
		}
	}
	net->sendMsg("true", newConnect);
	cout << "Пользователь с логином " << m_currentUser->getUserLogin() << " вошел в чат." << endl;
}

void ChatServer::CloseClient(int newConnect) {
	close(newConnect);
}

void ChatServer::signUp(int newConnect) {
	string login = net->getMsg(newConnect);
	string passHash = net->getMsg(newConnect);
	string name = net->getMsg(newConnect);
	if (getUserByLogin(login) || login == "all" || getUserByName(name) || name == "all") {
		net->sendMsg("login_not_uniq", newConnect);
	} else {
		string addUserToDB1 = "INSERT INTO users(login, name, password) VALUES ('" + login + "', '" + name + "', '0');";
		string addUserToDB2 = "INSERT INTO pwdhashes (user_id, pwdhash) SELECT id, '" + passHash + "' FROM users " +
			"WHERE login = '" + login + "';";
		db->sendRequest(addUserToDB1);
		db->sendRequest(addUserToDB2);
		net->sendMsg("success", newConnect);
		cout << "Пользователь с логином " + login + " зарегистрировался." << endl;
	}
}

void ChatServer::addMessage(int newConnect) {
	string text = net->getMsg(newConnect);
	string from = net->getMsg(newConnect);
	string to = net->getMsg(newConnect);
	string addMessageToDB = "INSERT INTO messages(msg_text, msg_from, msg_to, msg_timestamp) VALUES('" +
		text + "', '" + from + "', '" + to + "', UNIX_TIMESTAMP(now()));";
	db->sendRequest(addMessageToDB);
	net->sendMsg("success", newConnect);
	cout << "Сообщение от пользователя с логином " << getUserById(stoi(from))->getUserLogin() << " добавлено." << endl;
}

void ChatServer::readChatFromDB(int newConnect) {
	vector<string> usersTable, messagesTable;
	int num_rows = 0, num_fields = 0;
	string getUsersfromDB = "SELECT * FROM users ORDER BY id;";
	usersTable = db->getRequest(getUsersfromDB, num_rows, num_fields);
	net->sendMsg(to_string(num_rows), newConnect);
	int j = 0;
	for (int i = 0; i < num_rows; i++) {
		net->sendMsg(usersTable[j], newConnect);
		net->sendMsg(usersTable[j + 1], newConnect);
		net->sendMsg(usersTable[j + 2], newConnect);
		net->sendMsg(usersTable[j + 3], newConnect);
		net->sendMsg(usersTable[j + 4], newConnect);
		shared_ptr<User> user(new User(stoi(usersTable[j]), usersTable[j + 1], usersTable[j + 2], usersTable[j + 3], stoi(usersTable[j + 4])));
		m_users.push_back(user);
		j += num_fields;
	}
	string getMessagesfromBase = "SELECT * FROM messages ORDER BY id;";
	messagesTable = db->getRequest(getMessagesfromBase, num_rows, num_fields);
	net->sendMsg(to_string(num_rows), newConnect);
	j = 0;
	for (int i = 0; i < num_rows; i++) {
		net->sendMsg(messagesTable[j], newConnect);
		net->sendMsg(messagesTable[j + 1], newConnect);
		net->sendMsg(messagesTable[j + 2], newConnect);
		net->sendMsg(messagesTable[j + 3], newConnect);
		net->sendMsg(messagesTable[j + 4], newConnect);
		net->sendMsg(messagesTable[j + 5], newConnect);
		j += num_fields;
	}
}

#endif

void ChatServer::showUserList() const {
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

shared_ptr<User> ChatServer::getUserById(const int& id) const {
	for (auto& user : m_users) {
		if (id == user->getUserId()) return user;
	}
	return nullptr;
}

shared_ptr<User> ChatServer::getUserByLogin(const string& login) const {
	for (auto& user : m_users) {
		if (login == user->getUserLogin()) return user;
	}
	return nullptr;
}

std::shared_ptr<User> ChatServer::getUserByName(const std::string& name) const {
	for (auto& user : m_users) {
		if (name == user->getUserName()) return user;
	}
	return nullptr;
}
