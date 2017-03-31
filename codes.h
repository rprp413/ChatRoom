#include <string>

using std::string;

struct Error {
	char code;
	string description;
};

string Command[5] = {"MSG", "REGISTER", "LOGIN", "DISCONNECT", "CLIST"};

struct ClientInfo {
  string client_ID;
  string password;
};
