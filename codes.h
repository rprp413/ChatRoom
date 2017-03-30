#include <string>

struct Error {
	char code;
	string description;
};

string Command[5] = {"MSG", "REGISTER", "LOGIN", "DISCONNECT", "CLIST"};

struct ClientInfo {
	int portno;
  string client_ID;
  string password;
};
