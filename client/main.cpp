#include "chatclient.h"
using namespace std;

int main(int argc, char* argv[]) {
	set_cons();
	clear_screen();
	info();

	ChatClient* chatClient = new ChatClient();
	chatClient->readChat();
	while (chatClient->isWork()) {
		chatClient->initMenu();
	}
	delete chatClient;
	return 0;
}
