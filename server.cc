#include "server.h"
#include "file.h"
#include "codes.h"
#include "chatroom.h"
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
#include <pthread.h>

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

int Server::Scan() {
  if(pipe(pipefd) == -1) {
		perror("Failed to create the pipe");
		return 1;
	} 
  if(pipe(chatfd) == -1) {
		perror("Failed to create the pipe");
		return 1;
	}
	chatpid = fork();
	if(chatpid < 0) { // unsuccessful fork()
    perror("ERROR on fork");
    exit(EXIT_FAILURE);
  }
  if(chatpid == 0) { // child pid, this is the chatroom process
		// don't forget to not allow this to be a zombie!
    close(pipefd[0]);
    close(chatfd[1]);
    Chatroom chatroom;
		while(1) {
      //write(pipefd[1], "Connected to pipe successfully", 30);
      read(chatfd[0], msg, 1000);
      chatroom.CheckClient(msg);
      read(chatfd[0], msg, 1000);
      chatroom.Receive(msg, 1000);
      chatroom.Distribute(pipefd, chatfd);
    }
    // how do I kill the chatroom process?
  }
	else { // parent process
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
	}
  return void;
}

void Server::*RequestChatroom(void *arg) {
  
}

bool Server::Login(string client_ID, string password) {

}

bool Server::Register(string client_ID, string password) {

}


void Server::DealWithClient() {
  int error;
  pthread_t tid;
  if(error = pthread_create(&tid, NULL, RequestChatroom, NULL)) {
    perror("ERROR on creating a thread");
    return;
  }
  if(error = pthread_join(tid, NULL)) {
    perror("ERROR on joining thread");
    return;
  }


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
