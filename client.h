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

class Client {
public:
  void Setup();
  void Chat();
private:
	int sockfd;
	int portno;
	int n;
	struct sockaddr_in server_addr;
	struct hostent *server;
};

#endif // CLIENT_H_
