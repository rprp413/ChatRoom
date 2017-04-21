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
#include <signal.h>

#define BUFSIZE 1024


using namespace std;

/* In the constructor, make the program not be quitable by ^C
 *
*/ 
Client::Client() {
	if(sigemptyset(&sig) == -1 ||
		sigaddset(&sig, SIGINT) == -1 ||
		sigaddset(&sig, SIGQUIT) == -1) {
		perror("Failed to set up signal mask");
	}
	if(sigprocmask(SIG_BLOCK, &sig, NULL) == -1) {
		perror("Failed to block signals");
	}
}

/* "Server" thread of client that sends files requested
 *
*/
void *Scan(void *arg) {
	int serversocket = *(int *)arg;
	struct sockaddr_in client_addr;
	size_t client_length = sizeof(client_addr);
	char file_name[128];
	int bytes;
	while(1) {
// ACCEPT client connection
		int current_sock_fd;
		if((current_sock_fd = accept(serversocket, 
				  (struct sockaddr *) &client_addr, (socklen_t *) &client_length)) < 0) {
				    perror("ERROR on accept");
				    exit(EXIT_FAILURE);
		}
// READ in file name!
		if((bytes = read(current_sock_fd, file_name, 128)) < 0) {
			perror("Couldn't read filename");
			exit(EXIT_FAILURE);
		}
// OPEN file		
		FILE *fp = fopen(file_name, "rb");
		fseek(fp, 0L, SEEK_END);
		int sz = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		char size_of_file[16];
		sprintf(size_of_file, "%d", sz);
// SEND the file size!
		if((bytes = write(current_sock_fd, size_of_file, 16)) < 0) {
			perror("Couldn't send file size back!");
			exit(EXIT_FAILURE);
		}
// ALLOCATE buffer for file
		char file_buffer[sz];
// READ file into buffer
		if((bytes = fread(file_buffer, sizeof(char), sz, fp)) < 0) {
			perror("Error loading file into memory buffer");
			exit(EXIT_FAILURE);
		}
// WRITE buffer to client
		int bytes_written = 0;
		char *buffer_ptr = file_buffer;
		if((bytes_written = write(current_sock_fd, buffer_ptr, sz)) < 0) {
			perror("Error sending file bytes over socket");
			exit(EXIT_FAILURE);
		}
		fclose(fp);
		close(current_sock_fd);
	} 	// END WHILE
	return nullptr;
}

/* Setup sockets to 1) Server, 2) for other clients to connect to.
 *
*/
void Client::Setup() {
	// 1) Client portion for requests
  portno = 1065; // port numbers 0-1023 are reserved
  if((clientsockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Socket error");
    exit(EXIT_FAILURE);
  }
  client_addr.sin_family = AF_INET;
  client_addr.sin_port = htons(portno);
  client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
  if(connect(clientsockfd, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0) {
    perror("Connect error");
    exit(EXIT_FAILURE);
  }


	// 2) Server portion for file transfers
	if((serversockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("ERROR opening socket");
    exit(EXIT_FAILURE);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = 0;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  
  if(bind(serversockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
    perror("ERROR on binding");
    exit(EXIT_FAILURE);
  }

  if(listen(serversockfd, 5) < 0) {
    perror("ERROR on listening");
    exit(EXIT_FAILURE);
  }
	pthread_t thread;
	int error;
	while(error = pthread_create(&thread, NULL, Scan, &serversockfd)) {
			perror("Re-attempting to create thread for file handling");
	}
// GATHER data for ip address and PORT number for the client to know once doing FPUT command
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	getsockname(serversockfd, (struct sockaddr *)&sin, &len);
	serverportno = sin.sin_port;
	ip_address = sin.sin_addr.s_addr;
	cout << "IP Address: " << sin.sin_addr.s_addr << endl << "Port Number: " << sin.sin_port << endl;
	Chat();
}

unsigned char Client::ReadErrorCode() {
	if((n = read(clientsockfd, error_code, 1)) < 0) {
		perror("Couldn't receive error code from server");
		return 0xfe;
	}
	fprintf(stderr, "Server: 0x%x\n", error_code[0]);
	return error_code[0];
}

/* Reads in message. Note this thread is started,
 * then cancelled once fgets is called in Chat().
*/
void *ReadMSGHandler(void *args) {
	char read_buffer[1024];
	size_t bytes_read;
	int temp_socket = *(int*)args;
	// when main thread exits, the entire process ends and this thread also ends
	pthread_mutex_t printer = PTHREAD_MUTEX_INITIALIZER;
	while(1) {
		pthread_mutex_lock(&printer);	
		if((bytes_read = read(temp_socket, read_buffer, 1024)) < 0) {
			perror("Couldn't read message from Chatroom");
		}
		
		fprintf(stderr, "%s\n", read_buffer); // Instantly flush message out
		pthread_mutex_unlock(&printer);
	}
	return NULL;
}

/* Chat has two parts: 1) Logging in/Registering with the server
 * 2) Reading 
*/
void Client::Chat() {
	char read_buffer[512];
	if((n = read(clientsockfd, read_buffer, 512)) < 0) {
		perror("Couldn't read from socket");
		return;
	}
	char write_buffer[512];
	while(1) {

		cout << read_buffer << endl;
// INPUT
		fgets(write_buffer, 512, stdin); // reads the line including \n

		// Now get rid of ending new line character
		size_t ln = strlen(write_buffer) - 1;
		if( write_buffer[ln] == '\n') {
			write_buffer[ln] = '\0';
		}
// WRITE
		if((n = write(clientsockfd, write_buffer, 512)) < 0) {
			perror("Couldn't write to socket");
			return;
		}
		if(strncmp("DISCONNECT", write_buffer, 10) == 0) {
			// Disconnecting user, no error codes!
			close(clientsockfd);
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
	// NOTE: SIMILAR TO PREVIOUS WHILE LOOP
	// But first read what the Chatroom has to say to us!
	if((n = read(clientsockfd, read_buffer, 512)) < 0) {
		perror("Couldn't read from socket");
		return;
	}
  cout << read_buffer << endl; // display commands only once!
	// Create a separate thread to deal with receiving and displaying messages from the
	// server!


	int error;

	char msg_buffer[1024];
	pthread_mutex_t command_lock = PTHREAD_MUTEX_INITIALIZER;
	pthread_t thread;
	while(1) {
// CREATE THREAD
		while(error = pthread_create(&thread, NULL, ReadMSGHandler, &clientsockfd)) {
			perror("Re-attempting to create thread for message handling");
		}

		fgets(msg_buffer, 1024, stdin); // reads the line including
		
	// Now get rid of ending new line character
		size_t ln = strlen(msg_buffer) - 1;
		if( msg_buffer[ln] == '\n') {
			msg_buffer[ln] = '\0';
		}

		while(error = pthread_cancel(thread)) {
			perror("Re-attempting to cancel thread");
		}
// LOCK
		if(error = pthread_mutex_lock(&command_lock)) {
			perror("Error locking mutex");
			continue;
		}

// WRITE
		if((n = write(clientsockfd, msg_buffer, 1024)) < 0) {
			perror("Couldn't write to socket");
			return;
		}
		if(strncmp("DISCONNECT", msg_buffer, 10) == 0) {
			// Disconnecting user, no error codes!
			close(clientsockfd);
			cout << "Closed socket" << endl;
			return;
		}
// READ and FGET case
		else {
			ReadErrorCode();
			if(error_code[0] == 0) {
				if(strncmp("FGET", msg_buffer, 4) == 0) {
					char read_buffer[1024];
					size_t bytes_read;
// READ MSG, similar to ReadMSGHandler
					if((bytes_read = read(clientsockfd, read_buffer, 1024)) < 0) {
						perror("Couldn't read message from Chatroom");
					}
					char temp_ip[1024];
					int chars_read;
// Split the Ip address from the port number in these two for loops
					for(chars_read = 0; chars_read < 1024; chars_read++) {
						if(read_buffer[chars_read] == ' ') {
							break;
						}
						temp_ip[chars_read] = read_buffer[chars_read];
					}
					char temp_port[1024];
					int iter = 0;
					for(chars_read = chars_read + 1; chars_read < 1024; chars_read++) {
						if(read_buffer[chars_read] == '\0') {
							break;
						}
						temp_port[iter] = read_buffer[chars_read];
						iter++;
					}

// CONNECT to the other peer
					struct sockaddr_in temp_addr;
					int temp_sock;
					if((temp_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
   					perror("Socket error");
    				exit(EXIT_FAILURE);
  				}
					int conv_ip_address = (atoi(temp_ip)); // Don't need?
					int conv_port = htons(atoi(temp_port));
  				temp_addr.sin_family = AF_INET;
  				temp_addr.sin_port = htons(conv_port);
					inet_pton(AF_INET, temp_ip, &(temp_addr.sin_addr));
					if(connect(temp_sock, (struct sockaddr *) &temp_addr, sizeof(temp_addr)) < 0) {
						perror("Connect error");
						exit(EXIT_FAILURE);
					}
// WRITE to the peer the file name
					if((n = write(temp_sock, msg_buffer + 5, 128))  < 0) {
						perror("Couldn't write filename to other client");
						return;
					}
// READ the size of the file we requested
					int file_size;
					char size_reader[16];
					if((n = read(temp_sock, size_reader, 16)) < 0) {
						perror("Couldn't read file size from other client");
						return;
					}
					sscanf(size_reader, "%d", &file_size);
					file_size = atoi(size_reader);
// READ the file in
					char file_buffer[file_size];
					if((n = read(temp_sock, file_buffer, file_size)) < 0) {
							perror("Couldn't read from client");
							return;
					}
// CREATE the file and open for writing
					char file_name[128];
					strncpy(file_name, msg_buffer + 5, 128);
					FILE *fp = fopen(file_name, "wb");
					fwrite(file_buffer, 1, file_size, fp);
					fclose(fp);
					close(temp_sock);
				}
			}
		}
// UNLOCK		
		if(error = pthread_mutex_unlock(&command_lock)) {
			perror("Error unlocking mutex");
			continue;
		}


	} // END WHILE
 
	if(error = pthread_join(thread, NULL)) {
		perror("Error joining with thread");
	}
}
