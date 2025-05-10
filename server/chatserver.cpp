#include <iostream>
#include <filesystem>
#include <fstream>
#include "chatserver.h"
namespace fs = std::filesystem;
using namespace std;

ChatServer::ChatServer(string ip, int port) { 
	net = new NetServer(ip, port);
	net->start();
	m_isWork = true;
}

ChatServer::~ChatServer() {
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
		case RequestAPI::Registration:
			signUp(newSocket);
			cout << "Новый пользователь зарегистрировался." << endl;
			break;
		case RequestAPI::Login:
			login(newSocket);
			cout << "Пользователь " << m_currentUser->getUserName() << " вошел в чат." << endl;
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
	string password = net->getMsg(newSock);
	m_currentUser = getUserByLogin(login);
	if (m_currentUser == nullptr || (password != m_currentUser->getUserPassword())) {
		m_currentUser = nullptr;
		net->sendMsg("false", newSock);
	}
	net->sendMsg("true", newSock);
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
	string password = net->getMsg(newSocket);
	string name = net->getMsg(newSocket);
	if (getUserByLogin(login) || login == "all" || getUserByName(name) || name == "all") {
		net->sendMsg("login_not_uniq", newSocket);
	} else {
		shared_ptr<User> user(new User(login, password, name));
		m_users.push_back(user);
		writeChat();
		net->sendMsg("success", newSocket);
	}
}

void ChatServer::addMessage(SOCKET newSocket) {
	string text = net->getMsg(newSocket);
	string from = net->getMsg(newSocket);
	string to = net->getMsg(newSocket);
	m_messages.push_back(Message{ text, from, to, time(0) });
	writeChat();
	net->sendMsg("success", newSocket);
	cout << "Новое сообщение добавлено." << endl;
}

#elif defined (__linux__)
void ChatServer::clientHandler(int newConnect) {
	string codeMsg = "";
	while (codeMsg != to_string(RequestAPI::ServerExit)) {
		codeMsg = net->getMsg(newConnect);
		int oper = stoi(codeMsg);
		switch (oper) {
		case RequestAPI::Registration:
			signUp(newConnect);
			cout << "Новый пользователь зарегистрировался." << endl;
			break;
		case RequestAPI::Login:
			login(newConnect);
			cout << "Пользователь " << m_currentUser->getUserName() << " вошел в чат." << endl;
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
	string password = net->getMsg(newConnect);
	m_currentUser = getUserByLogin(login);
	if (m_currentUser == nullptr || (password != m_currentUser->getUserPassword())) {
		m_currentUser = nullptr;
		net->sendMsg("false", newConnect);
	}
	net->sendMsg("true", newConnect);
}

void ChatServer::CloseClient(int newConnect) {
	close(newConnect);
}

void ChatServer::signUp(int newConnect) {
	string login = net->getMsg(newConnect);
	string password = net->getMsg(newConnect);
	string name = net->getMsg(newConnect);
	if (getUserByLogin(login) || login == "all" || getUserByName(name) || name == "all") {
		net->sendMsg("login_not_uniq", newConnect);
	} else {
		shared_ptr<User> user(new User(login, password, name));
		m_users.push_back(user);
		writeChat();
		net->sendMsg("success", newConnect);
	}
}

void ChatServer::addMessage(int newConnect) {
	string text = net->getMsg(newConnect);
	string from = net->getMsg(newConnect);
	string to = net->getMsg(newConnect);
	m_messages.push_back(Message{ text, from, to, time(0) });
	writeChat();
	net->sendMsg("success", newConnect);
	cout << "Новое сообщение добавлено." << endl;
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

std::shared_ptr<User> ChatServer::getUserByLogin(const std::string& login) const {
	for (auto& user : m_users) {
		if (login == user->getUserLogin()) return user; // Исправлено по рекомендации проверяющего
	}
	return nullptr;
}

std::shared_ptr<User> ChatServer::getUserByName(const std::string& name) const {
	for (auto& user : m_users) {
		if (name == user->getUserName()) return user;
	}
	return nullptr;
}

void ChatServer::writeChat() {
	fstream user_file_wr = fstream(pathFile(U_FILE), ios::out); // Записываем пользователей в файл
	if (!user_file_wr)
		user_file_wr = fstream(pathFile(U_FILE), ios::out | ios::trunc);
	if (user_file_wr) {
		fs::permissions(pathFile(U_FILE), fs::perms::group_all | fs::perms::others_all, fs::perm_options::remove);
		for (auto& user : m_users)
			user_file_wr << user->getUserLogin() << "::" << user->getUserPassword() << "::" << user->getUserName() << endl;
	}
	user_file_wr.close();

	fstream message_file_wr = fstream(pathFile(M_FILE), ios::out); // Записываем сообщения в файл
	if (!message_file_wr)
		message_file_wr = fstream(pathFile(M_FILE), ios::out | ios::trunc);
	if (message_file_wr) {
		fs::permissions(pathFile(M_FILE), fs::perms::group_all | fs::perms::others_all, fs::perm_options::remove);
		for (auto& mess : m_messages)
			message_file_wr << mess.getTime() << "::" << mess.getFrom() << "::" << mess.getTo() << "::" << mess.getText() << endl;
	}
	message_file_wr.close();
}

void ChatServer::readChat() {
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
