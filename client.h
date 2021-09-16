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
  void Setup(string a_ipAddress);
  void Chat();
	unsigned char ReadErrorCode();
private:
   void getIPv4Address();
   string m_ipAddress;
   sigset_t sig;
	int m_clientSockFd;  ///< Socket FD for the client to connect to the server
	int m_serverSockFd;  ///< The client's server side for FPUTting a file
	int m_port;
	int m_serverPort;
	//uint32_t ip_address;
	ssize_t n;
	unsigned char error_code[1];
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	struct hostent *server;
};

#endif // CLIENT_H_
