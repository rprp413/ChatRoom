#include "chatroom.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using std::vector;
using std::string;
using std::stringstream;

Chatroom::Chatroom() {
}

int Chatroom::CheckClient(string client_ID, string password, int socket) {
	std::cout << "Size of clients: " << std::endl;
 if(clients.size() != 0) {
		for(vector<ClientInfo>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
		  if((client_ID == (*iter).client_ID) && (password == (*iter).password)) {
				std::cout << "Client already in chat!" << std::endl;
		    return 1;
		  }
		}
	}
  ClientInfo newClient = {client_ID, password, socket};
  clients.push_back(newClient);
	std::cout << "Added client!" << std::endl;
  return 0;
}

void Chatroom::DeleteClient(string client_ID) {
  if(clients.size() != 0) {
    for(vector<ClientInfo>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
      if( (*iter).client_ID == client_ID) {
        clients.erase(iter);
        return;
      }
    }
  }
}


void Chatroom::ReturnList(int client_socket, size_t chat_list_size) {
  char clist[chat_list_size];
  for(vector<ClientInfo>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
    strcat(clist, ((*iter).client_ID + " ").c_str());
  }
//	fprintf(stderr, "In chatoom: %s\n",clist);
  write(client_socket, clist, chat_list_size);
}

void Chatroom::Receive(char *recvmsg, int recvmsg_length, int pipefd[], int chatfd[]) {
  strcpy(msg, recvmsg);
  
  string input(recvmsg);
  stringstream ss(input);
  vector<string> s;
  string word;
  while(getline(ss, word, ' ')) {
    s.push_back(word);
  }
  if(*(s.begin()) == "MSG") {
    //Distribute(pipefd, chatfd);
  }

  else if(*(s.begin()) == "CLIST") {
   // ReturnList(pipefd);
  }
  else {
    // unknown command
  }
  msg_length = recvmsg_length;
}



void Chatroom::Distribute(char sending_msg[], size_t msg_size) {
//	fprintf(stderr, "%s\n", sending_msg);
	
  for(vector<ClientInfo>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
    write((*iter).socket, sending_msg, msg_size);
  }
}
