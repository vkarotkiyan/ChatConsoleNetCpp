#include "chatserver.h"

using namespace std;

int main(int argc, char* argv[]) {
	set_cons();
	clear_screen();
	info();

	ChatServer* chatServer = new ChatServer();

	while (chatServer->isWork()) {
		chatServer->initChat();
	}

	delete chatServer;
	return 0;
}
