#include "client.h"
#include <iostream>
using namespace std;

int main(int argc, char **argv) {
	Client client;
// Setup() calls Chat() so that "Server" thread is joined properly
	if(argc != 2)
	{
		printf("Format: ./client <ip_address>\n");
		return 0;
	}
	string ip_address = argv[1];
	cout << ip_address << endl;
	client.Setup(ip_address);
	return 0;
}
