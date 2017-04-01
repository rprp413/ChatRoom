#ifndef CODES_H_
#define CODES_H_

#include <string>

using std::string;

struct Error {
	char code;
	string description;
};

//string Command[5] = {"MSG", "REGISTER", "LOGIN", "DISCONNECT", "CLIST"};

struct ClientInfo {
  string client_ID;
  string password;
};

#endif // CODES_H_
