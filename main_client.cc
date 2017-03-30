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
#include <string.h>

using namespace std;
#define MAXBUFFER 256

int main(int argc, char **argv) {
  if(argc != 1) {
    perror("Format should be: ./client");
    exit(EXIT_FAILURE);
  }
  int sockfd, portno, n;
  portno = 1065; // port numbers 0-1023 are reserved
  struct sockaddr_in server_addr;
  struct hostent *server;
  char buffer[MAXBUFFER];
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket error");
    exit(EXIT_FAILURE);
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portno);
   
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
  if(connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
    perror("Connect error");
    exit(EXIT_FAILURE);
  }
  close(sockfd);

  return 0;
}
