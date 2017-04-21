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
#include <vector>
using namespace std;

#define MAXBUFFER 256
#define PORT 1065

struct arg_struct {
	int pid;
	int sockfd;
};

void *process_handler(void *arg){
	struct arg_struct *args = (struct arg_struct *) arg;
	cout << "Thread Created" << endl;
	int status;
	//int pid = *(int*)arg;
	waitpid(args->pid, &status, 0);
	close(args->sockfd);
	cout << "Closing sockets!" << endl;
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
	vector<pthread_t> threads;
	//pthread_t threads[100];
	int num_thread = 0; 
	vector<int> newsockfds; // don't really need it but for keeping track of all used...
	//int newsockfds[100];
	int num_sockfds = 0;
 while(1) {
		// accept is blocking, unless socket [sockfd] is marked as unblocking
		// in which case you need to check for EAGAIN error code!
	cout << "about to accept! " << num_sockfds << endl;
		int current_sock_fd;
    if(( current_sock_fd = accept(sockfd, 
        (struct sockaddr *) &client_addr, (socklen_t *) &client_length)) < 0) {
      perror("Fail on accept");
      exit(EXIT_FAILURE);
    }
	newsockfds.push_back(current_sock_fd);
	num_sockfds++; 
	cout << "accepted new client about to fork! " << num_sockfds << endl;
    pid = fork();
    if(pid < 0) { // unsuccessful fork()
      perror("FAIL on fork");
      exit(EXIT_FAILURE);
    }
    if(pid == 0) { // child process dealing with client
      close(sockfd);
      cout << "Dealing with new client! " << num_sockfds << endl;
			int x;
			char read_client[16];
			if((n = read(current_sock_fd, read_client, 16)) < 0) {
				perror("Couldn't read from client");
				exit(1);
			}
			if(read_client[0] == '0') {
				//cin >> x; // suspends process for tty input, don't do this!
      	exit(EXIT_SUCCESS);
			}
			else {
				cout << "Incorrect input from client" << endl;
				while(1) {}
			}
    }
  if(pid > 0) { // parent process
		struct arg_struct args;
		args.pid = pid;
		args.sockfd = current_sock_fd;
		pthread_t thread;
		pthread_create(&thread, NULL, process_handler, &args);
		cout << "Created thread, in parent " << num_sockfds << endl;
		threads.push_back(thread);
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
