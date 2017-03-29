#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
using namespace std;

#define MAXBUFFER 256
#define PORT 1024

int main(int argc, char **argv) {
  if(argc != 3) {
    cout << "Must be the following format: ./server port filename" << endl;
    exit(EXIT_FAILURE);
  }
  int sockfd, newsockfd, portno, client_length, pid, n;
  char buffer[MAXBUFFER];
  struct sockaddr_in server_addr, client_addr;
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("ERROR opening socket");
    exit(EXIT_FAILURE);
  }
  portno = atoi(argv[1]);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portno);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if(bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
    perror("ERROR on binding");
    exit(EXIT_FAILURE);
  }

  if(listen(sockfd, 5) < 0) {
    perror("Listen error");
    exit(EXIT_FAILURE);
  }
  while(1) {
		// accept is blocking, unless socket [sockfd] is marked as unblocking
		// in which case you need to check for EAGAIN error code!
    if((newsockfd = accept(sockfd, 
        (struct sockaddr *) &client_addr, (socklen_t *) &client_length)) < 0) {
      perror("Fail on accept");
      exit(EXIT_FAILURE);
    }
    pid = fork();
    if(pid < 0) { // unsuccessful fork()
      perror("FAIL on fork");
      exit(EXIT_FAILURE);
    }
    if(pid == 0) { // child process dealing with client
      close(sockfd);
      cout << "Dealing with new client!" << endl;
      exit(EXIT_SUCCESS);
    }
    else { // pid > 0, parent process
      close(newsockfd);
    }
  }
  return 0;
}
