#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
using namespace std;

#define MAXBUFFER 256
#define PORT 1024

void *process_handler(void *arg){
	cout << "Thread Created" << endl;
	int status;
	int pid = *(int*)arg;
	waitpid(pid, &status, 0);
	return arg;
}

int main(int argc, char **argv) {
  if(argc != 3) {
    cout << "Must be the following format: ./server port filename" << endl;
    exit(EXIT_FAILURE);
  }
  int sockfd, status, newsockfd, portno, client_length, pid, n;
  char buffer[MAXBUFFER];
  struct sockaddr_in server_addr, client_addr;
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("ERROR opening socket");
    exit(EXIT_FAILURE);
  }
  portno = atoi(argv[1]);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(portno);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if(bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
    perror("ERROR on binding");
    exit(EXIT_FAILURE);
  }

  if(listen(sockfd, 5) < 0) {
    perror("Listen error");
    exit(EXIT_FAILURE);
  }
	pthread_t threads[100];
	int num_thread = 0; 
 while(1) {
		// accept is blocking, unless socket [sockfd] is marked as unblocking
		// in which case you need to check for EAGAIN error code!
	cout << "about to accept!" << endl;
    if((newsockfd = accept(sockfd, 
        (struct sockaddr *) &client_addr, (socklen_t *) &client_length)) < 0) {
      perror("Fail on accept");
      exit(EXIT_FAILURE);
    }
	cout << "accepted new client about to fork!" << endl;
    pid = fork();
    if(pid < 0) { // unsuccessful fork()
      perror("FAIL on fork");
      exit(EXIT_FAILURE);
    }
    if(pid == 0) { // child process dealing with client
      close(sockfd);
      cout << "Dealing with new client!" << endl;
	int x;
	cin >> x; // suspends process for tty input
      exit(EXIT_SUCCESS);
    }
    else { // pid > 0, parent process
	pthread_create(&threads[num_thread], NULL, process_handler, &pid);
	cout << "Created thread, in parent" << endl;
      //waitpid(pid, &status, 0);
	num_thread++; 
	//cout << "Done waiting for child process!" << endl;
      //close(newsockfd);
    }
  }
	int i;
	for(i = 0; i < num_thread; i++) {
		pthread_join(threads[i], NULL);
	}
  return 0;
}
