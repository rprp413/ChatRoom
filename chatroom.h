#ifndef CHATROOM_H_
#define CHATROOM_H_

#include "codes.h"
#include <vector>
class Chatroom {
public:
  // Returns 1 if client is in chatroom, 0 if not and adds to chatroom
  int CheckClient(char *recvmsg);
  void DeleteClient(std::string client_ID);
	void Receive(char *recvmsg, int recvmsg_length);
	void Distribute(int pipefd[], int chatfd[]);
private:
  string temp_client_ID;
  string temp_password;
	char *msg;
  char *readmsg;
	int msg_length;
  std::vector<ClientInfo> clients;
};

#endif // CHATROOM_H_
