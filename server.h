#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include "codes.h"
#include "file.h"
#include <vector>

class Server {
public:
  void Setup(std::string file_name);
  int Scan();
  void GetInitialRequest(); // reads from client!
  void GetChatroomRequest();
  void DealWithClient();
  void *RequestChatroom(void *arg); // read from pipe for chatroom
  bool Login(std::string client_ID, std::string password);
  bool Register(std::string client_ID, std::string password);
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
  char *inchatmsg;
  char *msg;
  char *readchatmsg;
  std::vector<string> s;
  File file;
  ClientInfo client;
};

#endif // SERVER_H_
