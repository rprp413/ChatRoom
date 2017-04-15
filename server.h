#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include "codes.h"
#include "file.h"
#include "chatroom.h"
#include <vector>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <pthread.h>
using std::vector;
using std::string;
using std::thread;

class Server {
public:
  Server();
  void Setup(string file_name);
  int Scan();
	void GetFromClient(vector<string> &s, size_t msg_size);
  //void GetInitialRequest(int newsockfd); // reads from client!
  //void GetChatroomRequest(int newsockfd);
	void ErrorCode(unsigned char code);
  static void *DealWithClient(int newsockfd);
  static void *RequestChatroom(int clientsocket, char *read_chat_msg); // read from pipe for chatroom
	static void *CloseSocket(thread arg_thread, int arg_socket);
  bool Login(string client_ID, string password);
  bool Register(string client_ID, string password);
  void Disconnect();
	
private:
  int sockfd; // socket for scanning port
  int client_socket; // socket for each new port
  int portno; 
  int client_length; // length of client socket address
  int n;
  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  int numClients;
  int numThreads;
  char *inchatmsg;
  char msg[1000];
  char *readchatmsg;
	vector<thread> threads;
	static Chatroom chatroom;
  File file;
	
};

#endif // SERVER_H_
