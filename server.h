#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include "codes.h"
#include "file.h"
#include "chatroom.h"
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>

class Server {
public:
  Server();
  void Setup(std::string file_name);
  int Scan();
  void GetInitialRequest(); // reads from client!
  void GetChatroomRequest();
  void DealWithClient();
  static void *RequestChatroom(int pipefds[], int chatfds[], int clientsocket, char *read_chat_msg); // read from pipe for chatroom
  bool Login(std::string client_ID, std::string password);
  bool Register(std::string client_ID, std::string password);
  void Disconnect();
private:
  int sockfd; // socket for scanning port
  int newsockfd; // socket for each new port
  int portno; 
  int client_length; // length of client socket address
  int pid; // main pid
	int chatpid; // pid for chatroom creation
  int n;
  int status;
  int pipefd[2]; // for pipe between client and chatroom
  int chatfd[2]; // for pipe between chatroom and client
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
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
