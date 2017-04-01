#ifndef CHATROOM_H_
#define CHATROOM_H_

#include "codes.h"
#include <vector>
using std::vector;
using std::string;

class Chatroom {
public:
  // Returns 1 if client is in chatroom, 0 if not and adds to chatroom
  Chatroom();
  int CheckClient(char *recvmsg);
  void ReturnList(int pipefd[]);
  void DeleteClient(string client_ID);
	void Receive(char *recvmsg, int recvmsg_length, int pipefd[], int chatfd[]);
	void Distribute(int pipefd[], int chatfd[]);
private:
  string temp_client_ID;
  string temp_password;
	char *msg;
  char *readmsg;
	int msg_length;
  vector<ClientInfo> clients;
};

#endif // CHATROOM_H_
