#pragma once
#include <iostream> 
#include <string> 
#if defined (_WIN64) || defined(_WIN32)
#include <WS2tcpip.h>
#include <winsock2.h>
#pragma comment(lib, "WS2_32.lib")
#elif defined (__linux__)
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <netinet/in.h>
#endif

class NetServer {
public:
	NetServer(std::string ip, int port) : m_ip(ip), m_port(port) {};
	~NetServer() = default;
	void start();
#if defined (_WIN64) || defined(_WIN32)
	void sendMsg(std::string msg, SOCKET newConnect);
	std::string getMsg(SOCKET newConnect);
	SOCKET acception();
#elif defined(__linux__)
	void sendMsg(std::string msg, int newConnect);
	std::string getMsg(int newConnect);
	void acception();
	int getConnect();
#endif
	void Exit();
private:
	std::string m_ip;
	int m_port;
#if defined (_WIN64) || defined(_WIN32)
	WSADATA m_wsaData;
	SOCKET m_serverSocket;
	SOCKADDR_IN m_server_address;
#elif defined (__linux__)
	int m_socket_fd, m_connection;
	struct sockaddr_in m_server_address, m_client;
#endif
};
