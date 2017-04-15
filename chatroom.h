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
  int CheckClient(string client_ID, string password, int socket);
  void ReturnList(int client_socket, size_t chat_list_size);
  void DeleteClient(string client_ID);
	void Receive(char *recvmsg, int recvmsg_length, int pipefd[], int chatfd[]);
	void Distribute(const char sending_msg[], size_t msg_size);
private:
	char *msg;
  char *readmsg;
	int msg_length;
  vector<ClientInfo> clients;
};

#endif // CHATROOM_H_
