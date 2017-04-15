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

// C-style multi-threading
struct arg_struct {
	int pid;
	int sockfd;
};

void *process_handler(void *arg){
	struct arg_struct *args = (struct arg_struct *) arg;
	cout << "Thread Created" << endl;
	int status;
	// int pid = *(int*)arg; // example of correct type-casting with void
	waitpid(args->pid, &status, 0);
	close(args->sockfd);
	cout << "Closing sockets!" << endl;
	return arg;
}

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
	if(chatpid > 0) { // parent process
		vector<pthread_t> threads;
		int num_thread = 0;
		vector<int> newsockfds;
		int num_sock_fds = 0;
		while(1) {
		  // accept is blocking, unless socket [sockfd] is marked as unblocking
		  // in which case you need to check for EAGAIN error code!
			int current_sock_fd;
		  if((current_sock_fd = accept(sockfd,
		      (struct sockaddr *) &client_addr, (socklen_t *) &client_length)) < 0) {
		        perror("ERROR on accept");
		        exit(EXIT_FAILURE);
		  }
			newsockfds.push_back(current_sock_fd);
			num_sock_fds++;
		  pid = fork();
		  if(pid < 0) { // unsuccessful fork()
		    perror("ERROR on fork");
		    exit(EXIT_FAILURE);
		  }
		  if(pid == 0) { // child process dealing with client
		    close(sockfd);
		    cout << "Client connected!" << endl;
		    DealWithClient(current_sock_fd);
		    exit(EXIT_SUCCESS);
		  }
		  if(pid > 0) { // parent process
				struct arg_struct args;
				args.pid = pid;
				args.sockfd = current_sock_fd;
				pthread_t thread;
				int error;
				while(error = pthread_create(&thread, NULL, process_handler, &args)) {
					perror("Failed to create a thread!");
				}
				cout << "Created thread, in parent " << num_sock_fds << endl;
				threads.push_back(thread);
				num_thread++; 
		  }
		}
		for(int i = 0; i < num_thread; i++) {
			pthread_join(threads[i], NULL);
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
	cout << "No such client_ID or password is incorrect" << endl;
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

void Server::ErrorCode(unsigned char code) {
	unsigned char send_code[6] = {0, 1, 2, 3, 4, 255};	
	if( code == 0) {
		if((n = write(client_socket, send_code, 1)) < 0) {
			perror("Couldn't write error code to client");
			return;
		}
	} else if( code == 1) {
		if((n = write(client_socket, send_code + 1, 1)) < 0) {
			perror("Couldn't write error code to client");
			return;
		}
	} else if( code == 2) {
		if((n = write(client_socket, send_code + 2, 1)) < 0) {
			perror("Couldn't write error code to client");
			return;
		}
	} else if( code == 3) {
		if((n = write(client_socket, send_code + 3, 1)) < 0) {
			perror("Couldn't write error code to client");
			return;
		}
	} else if( code == 4) {
		if((n = write(client_socket, send_code + 4, 1)) < 0) {
			perror("Couldn't write error code to client");
			return;
		}
	} else if( code == 255) {
		if((n = write(client_socket, send_code + 5, 1)) < 0) {
			perror("Couldn't write error code to client");
			return;
		}
	} else {
			cout << "Invalid error code!" << endl;
	}
}

void Server::GetFromClient() {
	char read_client_msg[512];
  if((n = read(client_socket, read_client_msg, 512)) < 0) {
    perror("Couldn't read from socket");
    return;
  }
  s.clear();
  char *temp = strtok(read_client_msg, " ");
  while(temp != NULL) {
    s.push_back(string(temp));
    temp = strtok(NULL, " ");
  }
}

void Server::Disconnect() {}

void Server::DealWithClient(int newsockfd) {
	client_socket = newsockfd;
  char login = 0;
  char reg = 0;
	char write_msg1[512] = "Please respond with one of the following choices:\n 1) LOGIN <client_ID, password>\n 2) REGISTER    <client_ID, password>\n 3) DISCONNECT\n";
	if((n = write(client_socket, write_msg1, 512)) < 0) {
  	perror("Couldn't write to socket");
    return;
  }
  while(!login && !reg) {
		GetFromClient();
    // Login the user
		cout << *(s.begin()) << endl;
    if(*(s.begin()) == "LOGIN") {
			if(s.size() != 3) {
				ErrorCode(0xFF);
				cout << "Invalid format" << endl;
				continue;
			}
      if(Login(*(s.begin()+1), *(s.begin()+2))) {
        login = 1;
        client.client_ID = *(s.begin()+1);
        client.password = *(s.begin()+2);
				cout << "Successfully Logged in existing user!" << endl;
				ErrorCode(0);
      }
			else {
				ErrorCode(1);
			}
    }
    // Register the user
    else if(*(s.begin()) == "REGISTER") {
			if( s.size() != 3) {
				ErrorCode(0xFF);
				cout << "Invalid format" << endl;
				continue;
			}
      if(Register(*(s.begin()+1), *(s.begin()+2))) {
        reg = 1;
        client.client_ID = *(s.begin()+1);
        client.password = *(s.begin()+2);
				cout << "Successfully Registered new user!" << endl;
				ErrorCode(0);
      }
			else {
				ErrorCode(2);
			}
    }
    // make GetInitialRequest function!
    else if((*(s.begin()) == "DISCONNECT") && s.size() == 1) {
			// Must close all sockets for file transfer and server chatroom process must remove client_ID from the list
      Disconnect();
			return;
    }
		else {
			ErrorCode(0xFF);	
		}
  }


  thread tid(Server::RequestChatroom, pipefd, chatfd, client_socket, readchatmsg);
  
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

	// something must in client for this write to synchronize
	char write_msg2[512] = "Please respond with one of the following choices:\n 1) MSG <content>\n 2) CLIST\n 3) DISCONNECT\n\0";
	if((n = write(client_socket, write_msg2, 512)) < 0) {
  	perror("Couldn't write to socket");
    return;
  }

	// Pipe details:
	// chatfd[0] is read end of Chatroom up in Scan function
	// chatfd[1] is write end of Chatroom, to which we write to from here
	// pipefd[0] is read end of Client handler here
	// pipefd[1] is write end of Client handler here, to which we write to from Scan function
	// client_socket is member variable of Server, and is the client we are handling

  while(1) {
		GetFromClient();   
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
      write(client_socket, readchatmsg, 10000); // write to client
    }
    if(*(s.begin()) == "DISCONNECT") {
      Disconnect();
    }
    
  }
  if(tid.joinable()) {
    tid.join();
  }

}
