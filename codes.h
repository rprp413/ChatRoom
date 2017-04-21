#ifndef CODES_H_
#define CODES_H_
// This header file does not have a corresponding implementation .cc file
#include <string>

using std::string;

// Information regarding each new client
// This is managed by the Chatroom object
struct ClientInfo {
  string client_ID;
  string password;
	int socket;
};

// Information regarding each new file
// This is managed by the Chatroom object
struct FileData {
	string file_name;
	string client_ID;
	int port;
	char *ip_address;
};

#endif // CODES_H_
