#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include "codes.h"
#include "file.h"

class Server {
public:
  void Setup(string file_name);
  void Scan();
  void DealWithClient();
  bool Login(string client_ID, string password);
  bool Register(string client_ID, string password);
  void Disconnect();
private:
  int sockfd;
  int socket;
  int newsockfd;
  int portno;
  int client_length;
  int pid;
	int chatpid;
  int n;
  struct sockaddr_in server_addr;
  int numClients;
  File file; 
};

#endif // SERVER_H_
