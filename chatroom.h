#ifndef CHATROOM_H_
#define CHATROOM_H_

#include "codes.h"

class Chatroom {
public:
	void Receive(char *recvmsg, int recvmsg_length);
	void Distribute();
private:
	char *msg;
	int msg_length;
  ClientInfo clients[100];
};

#endif // CHATROOM_H_
