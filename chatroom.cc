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

Chatroom::Chatroom() {}

int Chatroom::CheckClient(char *recvmsg) {
  if(clients.size() != 0) {
    vector<string> s;
    char *temp = strtok(recvmsg, " ");
    while(temp != NULL) {
      s.push_back(string(temp));
      temp = strtok(NULL, " ");
    }
    temp_client_ID = *(s.begin());
    temp_password = *(s.begin()+1);

    for(vector<ClientInfo>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
      if((temp_client_ID == (*iter).client_ID) && (temp_password == (*iter).password)) {
        return 1;
      }
    }
    ClientInfo newClient = {temp_client_ID, temp_password};
    clients.push_back(newClient);
    return 0;
  }
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


void Chatroom::ReturnList(int pipefd[]) {
  char clist[10000] = {'0', 'x', '0', '0', ' '};
  for(vector<ClientInfo>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
    strcat(clist, ((*iter).client_ID + " ").c_str());
  }
  write(pipefd[1], clist, 10000);
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
    Distribute(pipefd, chatfd);
  }

  else if(*(s.begin()) == "CLIST") {
    ReturnList(pipefd);
  }
  else {
    // unknown command
  }
  msg_length = recvmsg_length;
}



void Chatroom::Distribute(int pipefd[], int chatfd[]) {
  for(int i = 0; i < clients.size(); i++) {
    write(pipefd[1], msg, sizeof(msg));
    read(chatfd[0], readmsg, 256);
  }
}
