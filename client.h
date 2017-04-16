#ifndef CLIENT_H_
#define CLIENT_H_

#include <string>
#include "codes.h"
#include "file.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

class Client {
public:
  Client();
  void Setup();
  void Chat();
	unsigned char ReadErrorCode();
private:
	sigset_t sig;
	int sockfd;
	int portno;
	int n;
	unsigned char error_code[1];
	struct sockaddr_in server_addr;
	struct hostent *server;
};

#endif // CLIENT_H_
