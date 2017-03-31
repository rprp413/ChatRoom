#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include "codes.h"
#include "file.h"

class Server {
public:
  void Setup(string file_name);
  int Scan();
  void DealWithClient();
  void *RequestChatroom(void *arg);
  bool Login(string client_ID, string password);
  bool Register(string client_ID, string password);
  void Disconnect();
private:
  int sockfd; // socket for scanning port
  int socket; // no idea
  int newsockfd; // socket for each new port
  int portno; 
  int client_length; // length of client socket address
  int pid; // main pid
	int chatpid; // pid for chatroom creation
  int n;
  int pipefd[2]; // for pipe between client and chatroom
  int chatfd[2]; // for pipe between chatroom and client
  struct sockaddr_in server_addr;
  int numClients;
  int numThreads;
  char *msg;
  File file; 
};

#endif // SERVER_H_
