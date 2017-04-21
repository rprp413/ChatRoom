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
using std::cout;
using std::endl;
using std::to_string;

Chatroom::Chatroom() {
}

// Compare string of file_name and send connection information to correct client
// Socket I/O handled by Chatroom object
void Chatroom::GetFile(string file_name, int client_socket, size_t msg_size) {
	if(files.size() != 0) {
		for(vector<FileData>::iterator iter = files.begin(); iter != files.end(); iter++) {
			if(file_name == (*iter).file_name) {
				char send_msg[msg_size];
				int temp_port = (*iter).port;
				strcat(send_msg, (*iter).ip_address);
				strcat(send_msg, (" " + to_string(temp_port) + "\0").c_str());
				write(client_socket, send_msg, msg_size);
				return;
			}
		}
	}
	return;
}

int Chatroom::AddFile(string file_name, int port, const char *ip_address) {
	if(CheckFile(file_name)) {
		return 0;
	}
	else {
		char *temp_ip = (char *) ip_address;
		FileData newFile = { file_name, port, temp_ip };
		files.push_back(newFile);
		return 1;
	}
}

// Checks the file against existing stored data on file_names handled by the server
// Used by AddFile
int Chatroom::CheckFile(string file_name) {
	if(files.size() != 0) {
		for(vector<FileData>::iterator iter = files.begin(); iter != files.end(); iter++) {
			if(file_name == (*iter).file_name) {
				return 1;
			}
		}
	}
	return 0;
}

// Doesn't delete file from any system, simply removes availability of file from server
void Chatroom::DeleteFile(string file_name) {
	if(files.size() != 0) {
		for(vector<FileData>::iterator iter = files.begin(); iter != files.end(); iter++) {
			if(file_name == (*iter).file_name) {
				files.erase(iter);
				return;
			}
		}
	}
}

void Chatroom::ReturnFileList(int client_socket, size_t chat_list_size) {
	char flist[chat_list_size];
	for(vector<FileData>::iterator iter = files.begin(); iter != files.end(); iter++) {
		strcat(flist, ((*iter).file_name + " ").c_str());
	}
	write(client_socket, flist, chat_list_size);
}


int Chatroom::CheckClient(string client_ID, string password, int socket) {
 if(clients.size() != 0) {
		for(vector<ClientInfo>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
		  if((client_ID == (*iter).client_ID) && (password == (*iter).password)) {
		    return 1;
		  }
		}
	}
  ClientInfo newClient = {client_ID, password, socket};
  clients.push_back(newClient);
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

void Chatroom::ReturnClientList(int client_socket, size_t chat_list_size) {
  char clist[chat_list_size];
  for(vector<ClientInfo>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
    strcat(clist, ((*iter).client_ID + " ").c_str());
  }
  write(client_socket, clist, chat_list_size);
}

void Chatroom::Distribute(char sending_msg[], size_t msg_size) {
	for(vector<ClientInfo>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
    write((*iter).socket, sending_msg, msg_size);
  }
}
