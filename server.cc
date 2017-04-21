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
#include <arpa/inet.h>

using namespace std;

Chatroom Server::chatroom;
// C-style multi-threading

/* This thread joins existing threads, specifically a DealWithClient thread
 * and closes the corresponding socket [or "newsockfd"]
*/
void *Server::CloseSocket(thread arg_thread, int arg_socket) {
	arg_thread.join();  // blocks until thread finishes!
	close(arg_socket);
	cout << "Closing sockets!" << endl;
	return NULL;
}

Server::Server() {}

/* Setting up the server port, note that the default port number is 1065
 * step-by-step: socket(), assign portnum, AF_INET and ip address, bind(),
 * listen(), and finally loop accept() for multiple clients [done in Scan()]
*/
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

/* Loops through and connects to a client through accept.
 * Each client connection gets a separate thread "DealWithClient" and 
 * gets encapsulated within another. This in turn gets pushed back into
 * a vector of threads, that gets joined at the end of Scan(), in case they don't 
 * finish.
*/
int Server::Scan() {
	while(1) {
	  // accept is blocking, unless socket [sockfd] is marked as unblocking
	  // in which case you need to check for EAGAIN error code!
		client_length = sizeof(client_addr);
		int current_sock_fd;
	  if((current_sock_fd = accept(sockfd,
	      (struct sockaddr *) &client_addr, (socklen_t *) &client_length)) < 0) {
	        perror("ERROR on accept");
	        exit(EXIT_FAILURE);
	  }
	  //thread client_thread(Server::DealWithClient, current_sock_fd);
		//thread closing_thread(Server::CloseSocket, client_thread, current_sock_fd);
		// this vector pushback way of making threads prevents copy constructors with threads!
		threads.push_back(thread(Server::CloseSocket, thread(Server::DealWithClient, current_sock_fd), current_sock_fd));
	}
	// Join threads that join other threads!
	for(int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}
  return 1;
}

// Returns 0 if login not possible!
bool Server::Login(string client_ID, string password) {
  file.SetFileName("accounts.dat");
  file.OpenIn();
  if(file.ReadFile(client_ID, password)) {
    cout << client_ID << " login successful" << endl; // make this some sort of wrtie to socket!
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

/* All possible error codes shown. Has been made easy to extend if 
 * the need arises to add more error codes.
*/
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

/* This function uses the Server objects string "s" and adds to it
 * the tokenized message (by space " "). msg_size is usually 1024.
*/
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


/* This is a thread. It has two parts:
 * 1) Assist with logging in and/or registering a new client
 * 2) Once logged in, manage requests
 * - Each request from the client must have an error code written back
 * immediately after the read from the client (except for "DISCONNECT")
 * - The chatroom object is statically global, thus it MUST be mutex
 * locked whenever you access it and unlocked immediately afterwards
 * - The server in general may have several output printing for 
 * confirmation of printing.
*/
void *Server::DealWithClient(int newsockfd) {
	Server temp_server;
	temp_server.client_socket = newsockfd;
	ClientInfo client;
	client.socket = newsockfd;
	vector<string> s;
	int bytes;
	size_t msg_size = 1024;

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

	// something must in client for this write to synchronize
	char write_msg2[512] = "Please respond with one of the following choices:\n 1) MSG <content>\n 2) CLIST\n 3) FLIST\n 4) FPUT <filename, IP_Address, port>\n 5) FGET <file_ID>\n 6) DISCONNECT\n\0";
	if((bytes = write(newsockfd, write_msg2, 512)) < 0) {
  	perror("Couldn't write to socket");
    return nullptr;
  }

	// Create a mutex for the Chatroom being locked!	
	mutex chatroom_mutex;

	chatroom_mutex.lock();

	Server::chatroom.CheckClient(client.client_ID, client.password, newsockfd);

	chatroom_mutex.unlock();

	char chat_msg[msg_size];

  while(1) {
		chatroom_mutex.lock();
		temp_server.GetFromClient(s, msg_size);
		chatroom_mutex.unlock();
    if(*(s.begin()) == "MSG") {
			string send_string = client.client_ID + ": ";
			// Retokenize the strings with spaces!
			for(int i = 1; i < s.size(); i++) {
				send_string += s[i] + " ";
			}
			int last = s.size() - 1;
			// Remove trailing white space for neatness
			while (last > 0 && s[last] == " ") {
				s.pop_back();
				last--;
			}
			strncpy(chat_msg, send_string.c_str(), msg_size);
			temp_server.ErrorCode(0);
			chatroom_mutex.lock();
			Server::chatroom.Distribute(chat_msg, msg_size);
			chatroom_mutex.unlock();
    }
    else if(*(s.begin()) == "CLIST") {
			if(s.size() != 1) {
				temp_server.ErrorCode(0xFF);
				cout << "Invalid Format" << endl;
				continue;
			}
			temp_server.ErrorCode(0);
			chatroom_mutex.lock();
			Server::chatroom.ReturnClientList(newsockfd, msg_size);
			chatroom_mutex.unlock();
    }
		else if(*(s.begin()) == "FPUT") {
			if(s.size() != 4) {
				temp_server.ErrorCode(0xFF);
				cout << "Invalid Format" << endl;
				continue;
			}
			// The following code reads and checks the port and ip address
			string::size_type sz;
			int temp_port = stoi(*(s.begin() + 3), &sz);
			struct sockaddr_in sa;
			int temp_ip_address = inet_pton(AF_INET, (*(s.begin() + 2)).c_str(), &(sa.sin_addr));
			if(temp_ip_address <= 0 || temp_port < 1024 || temp_port > 65535) {
				temp_server.ErrorCode(0x04);
				cout << "Invalid IP address and/or port" << endl;
				continue;
			}
			temp_server.ErrorCode(0);
			chatroom_mutex.lock();
			Server::chatroom.AddFile(*(s.begin() + 1), temp_port, (*(s.begin() + 2)).c_str(), client.client_ID);
			chatroom_mutex.unlock();
		}
		else if(*(s.begin()) == "FGET") {
			if(s.size() != 2) {
				temp_server.ErrorCode(0xFF);
				cout << "Invalid Format" << endl;
				continue;
			}
			chatroom_mutex.lock();
			int check = Server::chatroom.CheckFile(*(s.begin() + 1));
			chatroom_mutex.unlock();
			if(check == 0) {
				temp_server.ErrorCode(0x03);
				cout << "Invalid file_ID" << endl;
				continue;
			}
			temp_server.ErrorCode(0);
			chatroom_mutex.lock();
			Server::chatroom.GetFile(*(s.begin() + 1), newsockfd, msg_size);
			chatroom_mutex.unlock();
		}
		else if(*(s.begin()) == "FLIST") {
			if(s.size() != 1) {
				temp_server.ErrorCode(0xFF);
				cout << "Invalid Format" << endl;
				continue;
			}
			temp_server.ErrorCode(0);
			chatroom_mutex.lock();
			Server::chatroom.ReturnFileList(newsockfd, msg_size);
			chatroom_mutex.unlock();
		}
    else if(*(s.begin()) == "DISCONNECT") {
			chatroom_mutex.lock();
		// Delete the files first, then the client! VERY IMPORTANT!
			Server::chatroom.DeleteFiles(client.client_ID);
			Server::chatroom.DeleteClient(client.client_ID);
			chatroom_mutex.unlock();
      temp_server.Disconnect();
			return nullptr;
    }
		else {
			temp_server.ErrorCode(0xFF);
		}
    
  }
}
