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

unsigned char Client::ReadErrorCode() {
	if((n = read(sockfd, error_code, 1)) < 0) {
		perror("Couldn't receive error code from server");
		return 0xfe;
	}
	fprintf(stderr, "Server: 0x%x\n", error_code[0]);
	return error_code[0];
}

void *ReadMSGHandler(void *args) {
	char read_buffer[1024];
	size_t bytes_read;
	int temp_socket = *(int*)args;
	// when main thread exits, the entire process ends and this thread also ends
	pthread_mutex_t printer = PTHREAD_MUTEX_INITIALIZER;
	while(1) {
		if((bytes_read = read(temp_socket, read_buffer, 1024)) < 0) {
			perror("Couldn't read message from Chatroom");
		}
		pthread_mutex_lock(&printer);
		fprintf(stderr, "%s\n", read_buffer); // Instantly flush message out
		pthread_mutex_unlock(&printer);
	}
	return NULL;
}


void Client::Chat() {
	char read_buffer[512];
	if((n = read(sockfd, read_buffer, 512)) < 0) {
		perror("Couldn't read from socket");
		return;
	}
	char write_buffer[512];
	while(1) {

		cout << read_buffer << endl;

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
		if(strncmp("DISCONNECT", write_buffer, 10) == 0) {
			// Disconnecting user, no error codes!
			close(sockfd);
			cout << "Closed socket" << endl;
			return;
		}
		else {
			ReadErrorCode();
			if(error_code[0] == 0) {
				break;
			}
		}
	}



	// Now inside Chatroom! Have registered or logged in and can do commands!
	// NOTE: MAY BE SIMILAR TO PREVIOUS WHILE LOOP, ONCE FINISHED PERHAPS CONSOLIDATE!!!
	// But first read what the Chatroom has to say to us!
	if((n = read(sockfd, read_buffer, 512)) < 0) {
		perror("Couldn't read from socket");
		return;
	}
  cout << read_buffer << endl; // display commands only once!
	// Create a separate thread to deal with receiving and displaying messages from the
	// server!

// CREATE THREAD
	int error;
	pthread_t thread;
	while(error = pthread_create(&thread, NULL, ReadMSGHandler, &sockfd)) {
		perror("Re-attempting to create thread for message handling");
	}

	char write_buffer[1024];
	pthread_mutex_t command_lock = PTHREAD_MUTEX_INITIALIZER;

	while(1) {

		
		fgets(write_buffer, 512, stdin); // reads the line including

	// Now get rid of ending new line character
		size_t ln = strlen(write_buffer) - 1;
		if( write_buffer[ln] == '\n') {
			write_buffer[ln] = '\0';
		}

// LOCK
		if(error = pthread_mutex_lock(&command_lock)) {
			perror("Error locking mutex");
			continue;
		}
// WRITE
		if((n = write(sockfd, write_buffer, 512)) < 0) {
			perror("Couldn't write to socket");
			return;
		}
		if(strncmp("DISCONNECT", write_buffer, 10) == 0) {
			// Disconnecting user, no error codes!
			close(sockfd);
			cout << "Closed socket" << endl;
			return;
		}
// READ
		else {
			ReadErrorCode();
			if(error_code == 0) {
				break;
			}
		}
// UNLOCK		
		if(error = pthread_mutex_unlock(&command_lock) {
			perror("Error unlocking mutex");
			continue;
		}


	} // END WHILE

	if(error = pthread_join(thread, NULL)) {
		perror("Error joining with thread");
	}



}
