#include "client.h"
#include "file.h"
#include "codes.h"
#include "chatroom.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <vector>

using namespace std;

Client::Client() {

}

void Client::Setup() {
  portno = 1065; // port numbers 0-1023 are reserved
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket error");
    exit(EXIT_FAILURE);
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portno);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
  if(connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
    perror("Connect error");
    exit(EXIT_FAILURE);
  }
}

void Client::Chat() {
	char read_buffer[512];

	if((n = read(sockfd, read_buffer, 512)) < 0) {
		perror("Couldn't read from socket");
		return;
	}
	// char confirm[5] = {'0','x','0','0'};

	bool ok = false;
	while(!ok) {
		char recvconfirm[64];
		char write_buffer[512];
		cout << read_buffer << endl;
		// modify this portion for re-playing the choices and requesting again
		fgets(write_buffer, 512, stdin); // reads the line including \n

		// Now get rid of ending new line character
		size_t ln = strlen(write_buffer) - 1;
		if( write_buffer[ln] == '\n') {
			write_buffer[ln] = '\0';
		}
		if((n = write(sockfd, write_buffer, 512)) < 0) {
			perror("Couldn't write to socket");
			return;
		}
		if((n = read(sockfd, recvconfirm, 64)) < 0) {
			perror("Couldn't read from socket");
			return;
		}
		if(strcmp("0x00", recvconfirm) == 0) {
			ok = true;
			cout << "Server recv'd valid command!" << endl;
		}
	}
	close(sockfd);
	cout << "Closed socket" << endl;
}
