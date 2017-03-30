#include "server.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
using namespace std;

void Server::Setup(string file_name) {
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("ERROR opening socket");
    exit(EXIT_FAILURE);
  }
  portno = 1065;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portno);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  if(bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
    perror("ERROR on binding");
    exit(EXIT_FAILURE);
  }

  if(list(sockfd, 5) < 0) {
    perror("ERROR on listening");
    exit(EXIT_FAILURE);
  }

  return void;
}

void Server::Scan() {
  while(1) {
    // accept is blocking, unless socket [sockfd] is marked as unblocking
    // in which case you need to check for EAGAIN error code!

    if((newsockfd = accept(sockfd,
        (struct sockaddr *) &client_addr, (socklen_t *) &client_length)) < 0) {
          perror("ERROR on accept");
          exit(EXIT_FAILURE);
    }
    pid = fork();
    if(pid < 0) { // unsuccessful fork()
      perror("ERROR on fork");
      exit(EXIT_FAILURE);
    }
    if(pid == 0) { // child process dealing with client
      close(sockfd);
      cout << "Client connected!" << endl;
      DealWithClient();
      exit(EXIT_SUCCESS);
    }
    else { // pid > 0, parent process
      waitpid(pid, &status, 0);
      close(newsockfd);
    }
  }
  return void;
}

void Server::DealWithClient() {
  char read_msg[256];
  char write_msg[512] = {'Please respond with one of the following choices:\n 1) LOGIN <client_ID, password>\n 2) REGISTER <client_ID, password>\n 3) DISCONNECT\n\0'};
  if((n = write(newsockfd, msg, 256)) < 0) {
    perror("Couldn't write to socket");
    return;
  }
  if((n = read(newsockfd, msg, 256)) < 0) {
    perror("Couldn't read from socket");
    return;
  }


}



