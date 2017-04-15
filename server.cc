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
#include <mutex>

using namespace std;

Chatroom Server::chatroom;
// C-style multi-threading

void *Server::CloseSocket(thread arg_thread, int arg_socket) {
	cout << "Thread Created" << endl;
	arg_thread.join();  // blocks until thread finishes!
	close(arg_socket);
	cout << "Closing sockets!" << endl;
	return NULL;
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
	
	while(1) {
	  // accept is blocking, unless socket [sockfd] is marked as unblocking
	  // in which case you need to check for EAGAIN error code!
		int current_sock_fd;
	  if((current_sock_fd = accept(sockfd,
	      (struct sockaddr *) &client_addr, (socklen_t *) &client_length)) < 0) {
	        perror("ERROR on accept");
	        exit(EXIT_FAILURE);
	  }
	  cout << "Client connected!" << endl;
	  //thread client_thread(Server::DealWithClient, current_sock_fd);
		//thread closing_thread(Server::CloseSocket, client_thread, current_sock_fd);
		cout << "Created thread, in parent " << endl;
		// this vector pushback way of making threads prevents copy constructors with threads!
		threads.push_back(thread(Server::CloseSocket, thread(Server::DealWithClient, current_sock_fd), current_sock_fd));
	}
	// Join threads that join other threads!
	for(int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}
  return 1;
}

// This guy gets a message, and writes it to one
void* Server::RequestChatroom(int clientsocket, char *read_chat_msg) {
	cout << "In thread!" << endl;
  while(1) {
    //read(pipefds[0], read_chat_msg, 1024);
   // write(clientsocket, read_chat_msg, 1024);
  }
  return nullptr;
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

void Server::GetFromClient(vector<string> &s, size_t msg_size) {
	char read_client_msg[msg_size];
  if((n = read(client_socket, read_client_msg, msg_size)) < 0) {
    perror("Couldn't read from socket");
    return;
  }
  s.clear();
  char *temp = strtok(read_client_msg, " ");
  while(temp != NULL) {
    s.push_back(string(temp));
    temp = strtok(NULL, " ");
  }
	return;
}

void Server::Disconnect() {}

void *Server::DealWithClient(int newsockfd) {
	Server temp_server;
	temp_server.client_socket = newsockfd;
	ClientInfo client;
	client.socket = newsockfd;
	vector<string> s;
	int bytes;
	size_t msg_size = 1024;
	size_t chat_size = 10240;

  char login = 0;
  char reg = 0;
	char write_msg1[512] = "Please respond with one of the following choices:\n 1) LOGIN <client_ID, password>\n 2) REGISTER    <client_ID, password>\n 3) DISCONNECT\n";
	if((bytes = write(newsockfd, write_msg1, 512)) < 0) {
  	perror("Couldn't write to socket");
    return nullptr;
  }
  while(!login && !reg) {
		temp_server.GetFromClient(s, 512);
    // Login the user
		cout << *(s.begin()) << endl;
    if(*(s.begin()) == "LOGIN") {
			if(s.size() != 3) {
				temp_server.ErrorCode(0xFF);
				cout << "Invalid format" << endl;
				continue;
			}
      if(temp_server.Login(*(s.begin()+1), *(s.begin()+2))) {
        login = 1;
        client.client_ID = *(s.begin()+1);
        client.password = *(s.begin()+2);
				cout << "Successfully Logged in existing user!" << endl;
				temp_server.ErrorCode(0);
      }
			else {
				temp_server.ErrorCode(1);
			}
    }
    // Register the user
    else if(*(s.begin()) == "REGISTER") {
			if( s.size() != 3) {
				temp_server.ErrorCode(0xFF);
				cout << "Invalid format" << endl;
				continue;
			}
      if(temp_server.Register(*(s.begin()+1), *(s.begin()+2))) {
        reg = 1;
        client.client_ID = *(s.begin()+1);
        client.password = *(s.begin()+2);
				cout << "Successfully Registered new user!" << endl;
				temp_server.ErrorCode(0);
      }
			else {
				temp_server.ErrorCode(2);
			}
    }
    // make GetInitialRequest function!
    else if((*(s.begin()) == "DISCONNECT") && s.size() == 1) {
			// Must close all sockets for file transfer and server chatroom process must remove client_ID from the list
      temp_server.Disconnect();
			return nullptr;
    }
		else {
			temp_server.ErrorCode(0xFF);	
		}
  }


  //thread tid(Server::RequestChatroom, newsockfd, readchatmsg);
  
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
	if((bytes = write(newsockfd, write_msg2, 512)) < 0) {
  	perror("Couldn't write to socket");
    return nullptr;
  }

	// Create a mutex for the Chatroom being locked!	
	mutex chatroom_mutex;
	cout << "Before locking" << endl;
	chatroom_mutex.lock();
	cout << "Locked" << endl;
	Server::chatroom.CheckClient(client.client_ID, client.password, newsockfd);
	cout << "After client check" << endl;
	chatroom_mutex.unlock();
	cout << "After unlocking" << endl;

	char chat_msg[msg_size];

  while(1) {
		chatroom_mutex.lock();
		temp_server.GetFromClient(s, msg_size);
		chatroom_mutex.unlock();
    if(*(s.begin()) == "MSG") {
			string send_string = client.client_ID + ": ";
			for(int i = 1; i < s.size(); i++) {
				send_string += s[i] + " ";
			}
			int last = s.size() - 1;
			while (last > 0 && s[last] == " ") {
				s.pop_back();
				last--;
			}
			strncpy(chat_msg, send_string.c_str(), msg_size);
			cout << send_string << endl;
			chatroom_mutex.lock();
			temp_server.ErrorCode(0);
			chatroom_mutex.unlock();

			chatroom_mutex.lock();
			Server::chatroom.Distribute(chat_msg, msg_size);
			cout << "distributed!" << endl;
			chatroom_mutex.unlock();
			
    }
    if(*(s.begin()) == "CLIST" && s.size() == 1) {

			chatroom_mutex.lock();
			Server::chatroom.ReturnList(newsockfd, chat_size);
			chatroom_mutex.unlock();
			temp_server.ErrorCode(0);
      //char temp[chat_size];
      //strcpy(temp, client.client_ID.c_str());
      //strcat(temp, client.password.c_str());
     // write(chatfd[1], temp, chat_size);
      //write(chatfd[1], msg, chat_size); // request from chatroom a list of clients
     // read(pipefd[0], readchatmsg, 10000); // receive from chatroom a list of clients
      //write(newsockfd, readchatmsg, 10000); // write to client
    }
    if(*(s.begin()) == "DISCONNECT") {
			Server::chatroom.DeleteClient(client.client_ID);
      temp_server.Disconnect();
			return nullptr;
    }
    
  }
/*
  if(tid.joinable()) {
    tid.join();
  }
*/
}
