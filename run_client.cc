#include "client.h"
#include <iostream>
using namespace std;

int main() {
	Client client;
// Setup() calls Chat() so that "Server" thread is joined properly
	client.Setup();
	return 0;
}
