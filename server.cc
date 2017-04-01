#include "server.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <thread>
#include <vector>
#include <pthread.h>


using namespace std;

Server::Server() {}

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

  if(listen(sockfd, 5) < 0) {
    perror("ERROR on listening");
    exit(EXIT_FAILURE);
  }
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
      // write(pipefd[1], "Connected to pipe successfully", 30);
      read(chatfd[0], inchatmsg, 1000);
      chatroom.CheckClient(inchatmsg);
      read(chatfd[0], inchatmsg, 1000);
      chatroom.Receive(inchatmsg, 1000, pipefd, chatfd);
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
  return 1;
}

// read from chatroom messages from other clients
void* Server::RequestChatroom(int pipefds[], int chatfds[], int clientsocket, char *read_chat_msg) {

  while(1) {
    read(pipefds[0], read_chat_msg, 1000);
    write(clientsocket, read_chat_msg, 1000);
  }
  return NULL;
}


// Returns 0 if login not possible!
bool Server::Login(string client_ID, string password) {
  file.SetFileName("accounts.dat");
  file.OpenIn();
  if(file.ReadFile(client_ID, password)) {
    cout << "Login Successful" << endl; // make this some sort of wrtie to socket!
    file.CloseFile();
    return 1;
  }
  file.CloseFile();
  return 0;
}

// Returns 0 if account is already registered!
bool Server::Register(string client_ID, string password) {
  file.SetFileName("accounts.dat");
  file.OpenIn();
  file.OpenOut();
  if(file.CheckID(client_ID)) {
    cout << "Account already exists!" << endl;
    file.CloseFile();
    return 0;
  }
  file.WriteFile(client_ID, password);
  file.CloseFile();
  return 1;
}

void Server::GetInitialRequest() {
  char write_msg[512] = "Please respond with one of the following choices:\n 1) LOGIN <client_ID, password>\n 2) REGISTER    <client_ID, password>\n 3) DISCONNECT\n\0";
  if((n = write(newsockfd, write_msg, 256)) < 0) {
    perror("Couldn't write to socket");
    return;
  }
  if((n = read(newsockfd, msg, 256)) < 0) {
    perror("Couldn't read from socket");
    return;
  }
  s.clear();
  char *temp = strtok(msg, " ");
  while(temp != NULL) {
    s.push_back(string(temp));
    temp = strtok(NULL, " ");
  }
}


void Server::GetChatroomRequest() {
  char write_msg[512] = "Please respond with one of the following choices:\n 1) MSG <content>\n 2) CLIST\n 3) DISCONNECT\n\0";
  if((n = write(newsockfd, write_msg, 256)) < 0) {
    perror("Couldn't write to socket");
    return;
  }
  if((n = read(newsockfd, msg, 1000)) < 0) {
    perror("Couldn't read from socket");
    return;
  }
  s.clear();
  char *temp = strtok(msg, " ");
  while(temp != NULL) {
    s.push_back(string(temp));
    temp = strtok(NULL, " ");
  }
}

void Server::Disconnect() {}

void Server::DealWithClient() {


  char login = 0;
  char reg = 0;

  while(!login && !reg) {
    GetInitialRequest(); // get msg from client
    // Login the user
    if((*(s.begin()) == "LOGIN") && s.size() == 3) {
      if(Login(*(s.begin()+1), *(s.begin()+2))) {
        login = 1;
        client.client_ID = *(s.begin()+1);
        client.password = *(s.begin()+2);
      }
    }
    // Register the user
    if((*(s.begin()) == "REGISTER") && s.size() == 3) {
      if(Register(*(s.begin()+1), *(s.begin()+2))) {
        reg = 1;
        client.client_ID = *(s.begin()+1);
        client.password = *(s.begin()+2);
      }
    }
    // make GetInitialRequest function!
    if((*(s.begin()) == "DISCONNECT") && s.size() == 1) {
      Disconnect();
    }
  }


  thread tid(Server::RequestChatroom, pipefd, chatfd, newsockfd, readchatmsg);
  
/*
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
  */
  while(1) {
    GetChatroomRequest();
    if(*(s.begin()) == "MSG") {
      char temp[1000];
      strcpy(temp, client.client_ID.c_str());
      strcat(temp, client.password.c_str());
      write(chatfd[1], temp, 1000);
      write(chatfd[1], msg, 1000);
    }
    if(*(s.begin()) == "CLIST") {
      char temp[1000];
      strcpy(temp, client.client_ID.c_str());
      strcat(temp, client.password.c_str());
      write(chatfd[1], temp, 1000);
      write(chatfd[1], msg, 1000); // request from chatroom a list of clients
      read(pipefd[0], readchatmsg, 10000); // receive from chatroom a list of clients
      write(newsockfd, readchatmsg, 10000); // write to client
    }
    if(*(s.begin()) == "DISCONNECT") {
      Disconnect();
    }
    
  }
  if(tid.joinable()) {
    tid.join();
  }

}
