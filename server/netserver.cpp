#include "netserver.h"
using namespace  std;

void NetServer::start() {
#if defined(_WIN64) || defined(_WIN32)	
	int erStat(0);
	erStat = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	if (erStat != 0) {
		cout << "Ошибка инициализации версии Winsock #" << WSAGetLastError() << endl;
		return;
	}
	m_serverSocket = socket(AF_INET, SOCK_STREAM, 0); // Инициализация сокета
	if (m_serverSocket == INVALID_SOCKET) {
		cout << "Ошибка инициализации сокета #" << WSAGetLastError() << endl;
		Exit();
	}
	ZeroMemory(&m_server_address, sizeof(m_server_address)); // Инициализация адреса
	in_addr ip_to_num{};
	inet_pton(AF_INET, m_ip.c_str(), &ip_to_num);
	m_server_address.sin_addr = ip_to_num;
	m_server_address.sin_family = AF_INET;
	m_server_address.sin_port = htons(m_port);

	erStat = bind(m_serverSocket, (sockaddr*)&m_server_address, sizeof(m_server_address)); //bind
	if (erStat != 0) {
		cout << "Ошибка привязки сокета к информации о сервере. Ошибка # " << WSAGetLastError() << endl;
		Exit();
		return;
	}
	erStat = listen(m_serverSocket, SOMAXCONN);
	if (erStat != 0) {
		cout << "Не может начать просушивание. Ошибка # " << WSAGetLastError() << endl;
		Exit();
		return;
	}
#elif defined(__linux__)
	int erStat(0);
	m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket_fd == -1) {
		cout << "ERROR: Ошибка создания сокета." << endl;
		exit(1);
	}
	m_server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	m_server_address.sin_port = htons(m_port);
	m_server_address.sin_family = AF_INET;
	erStat = bind(m_socket_fd, (struct sockaddr*)&m_server_address, sizeof(m_server_address));
	if (erStat == -1) {
		cout << "ERROR: Ошибка привязки сокета." << endl;
		exit(1);
	}
	erStat = listen(m_socket_fd, SOMAXCONN);
	if (erStat == -1) {
		cout << "ERROR: Ошибка при постановке на приём данных." << endl;
		exit(1);
	}
#endif	
	cout << "Слушает порт " << m_port << "..." << endl;
}

#if defined(_WIN64) || defined(_WIN32)
void NetServer::sendMsg(std::string msg, SOCKET newConnect) {
	int packet_size;
	int msg_size = msg.size();
	packet_size = send(newConnect, (char*)&msg_size, sizeof(int), 0);
	if (packet_size == SOCKET_ERROR) {
		cout << "Не удается послать размер сообщения клиенту. Ошибка #" << WSAGetLastError() << endl;
		Exit();
		return;
	}
	packet_size = send(newConnect, msg.c_str(), msg_size, 0);
	if (packet_size == SOCKET_ERROR) {
		cout << "Не удается послать сообщение клиенту. Ошибка #" << WSAGetLastError() << endl;
		Exit();
		return;
	}
}

std::string NetServer::getMsg(SOCKET newConnect) {
	int packet_size;
	int msg_size(0);
	string msg = "";
	packet_size = recv(newConnect, (char*)&msg_size, sizeof(int), 0);
	if (packet_size == SOCKET_ERROR) {
		cout << "Не удается получить размер сообщения от клиента. Ошибка # " << WSAGetLastError() << endl;
		Exit();
		return msg;
	}
	char* answer = new char[msg_size + 1];
	answer[msg_size] = '\0';
	packet_size = recv(newConnect, answer, msg_size, 0);
	msg = answer;
	delete[] answer;
	if (packet_size == SOCKET_ERROR) {
		cout << "Не удается получить сообщение от клиента. Ошибка # " << WSAGetLastError() << endl;
		Exit();
		return msg;
	}
	return msg;
}

SOCKET NetServer::acception() {
	sockaddr_in client;
	ZeroMemory(&client, sizeof(client));
	int clientSize = sizeof(client);
	SOCKET newConn = accept(m_serverSocket, (sockaddr*)&client, &clientSize);
	if (newConn == INVALID_SOCKET) {
		cout << "Клиент обнаружен, но подключиться к нему не удается. Ошибка # " << WSAGetLastError() << endl;
		closesocket(newConn);
	}
	return newConn;
}

#elif defined (__linux__)
void NetServer::sendMsg(std::string msg, int newConnect) {
	int packet_size;
	int msg_size = msg.size();
	packet_size = send(newConnect, (char*)&msg_size, sizeof(int), 0);
	if (packet_size == -1) {
		cout << "ERROR: Не удается послать размер сообщения клиенту." << endl;
		Exit();
		return;
	}
	packet_size = send(newConnect, msg.c_str(), msg_size, 0);
	if (packet_size == -1) {
		cout << "ERROR: Не удается послать сообщение клиенту." << endl;
		Exit();
		return;
	}
}

std::string NetServer::getMsg(int newConnect) {
	int packet_size;
	int msg_size(0);
	string msg = "";
	packet_size = recv(newConnect, (char*)&msg_size, sizeof(int), 0);
	if (packet_size == -1) {
		cout << "ОШИБКА: Не удается получить размер сообщения от клиента." << endl;
		Exit();
		return msg;
	}
	char* answer = new char[msg_size + 1];
	answer[msg_size] = '\0';
	packet_size = recv(newConnect, answer, msg_size, 0);
	msg = answer;
	delete[] answer;
	if (packet_size == -1) {
		cout << "ОШИБКА: Не удается получить сообщение от клиента." << endl;
		Exit();
		return msg;
	}
	return msg;
}

void NetServer::acception() {
	sockaddr_in client;
	socklen_t length = sizeof(m_client);
	m_connection = accept(m_socket_fd, (struct sockaddr*)&m_client, &length);
	if (m_connection == -1) {
		cout << "ERROR: Сервер не смог принять данные от клиента." << endl;
		exit(1);
	}
}

int NetServer::getConnect() {
	return m_connection;
}
#endif

void NetServer::Exit() {
#if defined(_WIN64) || defined(_WIN32)
	closesocket(m_serverSocket);
	WSACleanup();
#elif defined (__linux__)
	close(m_socket_fd);
#endif
}
