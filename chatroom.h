#ifndef CHATROOM_H_
#define CHATROOM_H_

#include "codes.h"
#include <vector>
using std::vector;
using std::string;

class Chatroom {
public:
  // Returns 1 if client is in chatroom, 0 if not and adds to chatroom
  Chatroom();
  int CheckClient(string client_ID, string password, int socket);
  void ReturnClientList(int client_socket, size_t chat_list_size);
  void DeleteClient(string client_ID);
	void Distribute(char sending_msg[], size_t msg_size);
	int AddFile(string file_name, int port, const char *ip_address, string client_ID);
	int CheckFile(string file_name);
	void GetFile(string file_name, int client_socket, size_t msg_size);
	void DeleteFiles(string client_ID);
	void ReturnFileList(int client_socket, size_t chat_list_size);
private:
	char *msg;
  char *readmsg;
	int msg_length;
  vector<ClientInfo> clients;
	vector<FileData> files;
};

#endif // CHATROOM_H_
