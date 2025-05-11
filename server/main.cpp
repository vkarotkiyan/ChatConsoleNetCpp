#include "chatserver.h"

using namespace std;

int main(int argc, char* argv[]) {
	set_cons();
	clear_screen();
	info();

	ChatServer* chatServer = new ChatServer("127.0.0.1", 7777);

	chatServer->readChat();
	while (chatServer->isWork()) {
		chatServer->initChat();
	}
	chatServer->writeChat();

	delete chatServer;
	return 0;
}
