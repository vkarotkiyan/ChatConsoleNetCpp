#include "net.h"
using namespace std;

void NetClient::start() {
#if defined(_WIN64) || defined(_WIN32)	
	int erStat;
	erStat = WSAStartup(MAKEWORD(2, 2), &m_wsaData); // Иициализация Winsock
	if (erStat != 0) {
		cout << "Ошибка инициализации версии Winsock #" << WSAGetLastError() << endl;
		return;
	}
	m_clientSocket = socket(AF_INET, SOCK_STREAM, 0); // Иициализация сокета
	if (m_clientSocket == INVALID_SOCKET) {
		cout << "Ошибка инициализации сокета #" << WSAGetLastError() << endl;
		Exit();
	}
	ZeroMemory(&m_server_address, sizeof(m_server_address)); // Инициализация серверной информации
	in_addr ip_to_num{};
	inet_pton(AF_INET, m_ip.c_str(), &ip_to_num);
	m_server_address.sin_addr = ip_to_num;
	m_server_address.sin_family = AF_INET;
	m_server_address.sin_port = htons(m_port);
	erStat = connect(m_clientSocket, (sockaddr*)&m_server_address, sizeof(m_server_address)); // Соединение с сервером
	if (erStat != 0) {
		cout << "Ошибка соединения с сервером #" << WSAGetLastError() << endl;
		Exit();
		return;
	}
#elif defined(__linux__)
	m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket_fd == -1) {
		cout << "ОШИБКА: Ошибка создания сокета." << endl;
		exit(1);
	}
	m_server_address.sin_addr.s_addr = inet_addr(m_ip.data());
	m_server_address.sin_port = htons(m_port);
	m_server_address.sin_family = AF_INET;
	m_connection = connect(m_socket_fd, (struct sockaddr*)&m_server_address, sizeof(m_server_address));
	if (m_connection == -1)	{
		cout << "ОШИБКА: Ошибка подключения к серверу." << endl;
		exit(1);
	}
#endif
	cout << "Подключение к серверу прошло успешно." << endl;
}

void NetClient::sendMsg(string& msg) {
	int packet_size;
	int msg_size = msg.size();
#if defined(_WIN64) || defined(_WIN32)
		packet_size = send(m_clientSocket, (char*)&msg_size, sizeof(int), 0);
#elif defined(__linux__)
	packet_size = send(m_socket_fd, (char*)&msg_size, sizeof(int), 0);
#endif
		if (packet_size == -1) {
		cout << "ОШИБКА: Не удается послать размер сообщения серверу." << endl;
		Exit();
		return;
	}
#if defined(_WIN64) || defined(_WIN32)
		packet_size = send(m_clientSocket, msg.c_str(), msg_size, 0);
#elif defined(__linux__)
		packet_size = send(m_socket_fd, msg.c_str(), msg_size, 0);
#endif
	if (packet_size == -1) {
		cout << "ОШИБКА: Не удается послать сообщение серверу." << endl;
		Exit();
		return;
	}
}

string NetClient::getMsg() {
	int packet_size;
	int msg_size(0);
	string msg = "";
#if defined(_WIN64) || defined(_WIN32)
	packet_size = recv(m_clientSocket, (char*)&msg_size, sizeof(int), 0);
#elif defined(__linux__)
	packet_size = recv(m_socket_fd, (char*)&msg_size, sizeof(int), 0);
#endif
	if (packet_size == -1) {
		cout << "ОШИБКА: Не удается получить размер сообщения от сервера." << endl;
		Exit();
		return msg;
	}
	char* answer = new char[msg_size + 1];
	answer[msg_size] = '\0';
#if defined(_WIN64) || defined(_WIN32)
	packet_size = recv(m_clientSocket, answer, msg_size, 0);
#elif defined (__linux__)
	packet_size = recv(m_socket_fd, answer, msg_size, 0);
#endif
	msg = answer;
	delete[] answer;
	if (packet_size == -1) {
		cout << "ОШИБКА. Не удается получить сообщение от сервера." << endl;
		Exit();
		return msg;
	}
	return msg;
}

void NetClient::Exit() {
#if defined(_WIN64) || defined(_WIN32)
	closesocket(m_clientSocket);
	WSACleanup();
#elif defined (__linux__)
	close(m_socket_fd);
#endif
}
