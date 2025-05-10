#include "net.h"
#include "chatclient.h"
using namespace std;

int main(int argc, char* argv[]) {
	set_cons();
	clear_screen();
	info();

	ChatClient* chatClient = new ChatClient("127.0.0.1", 7777);
	chatClient->readChat();
	while (chatClient->isWork()) {
		chatClient->initMenu();
	}
	delete chatClient;
	return 0;
}
