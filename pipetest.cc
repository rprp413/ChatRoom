#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <stdlib.h>
#define BUFSIZE 1000



void chatroom(int pipefd[], int chatfd[], char *bufferin, char *bufferout) {
    close(chatfd[1]);
    close(pipefd[0]);
    for(int i = 0; i < 5; i++){
      write(pipefd[1], "Please respond with one of the following choices:\n 1) LOGIN <client_ID, password>\n 2) REGISTER <client_ID, password>\n 3) DISCONNECT\n\0", 512);
      read(chatfd[0], bufferin, sizeof(bufferin));
      fprintf(stderr, "Message from child is: %s\n", bufferin);
    }
    fprintf(stderr, "Chatroom pid is %d\n", (int)getpid());
  return;
}





int main(void) {
	char bufin[BUFSIZE] = "empty";
	char bufout[] = "hello";
	int bytesin;
	pid_t childpid, pid, wpid;
	int fd[2], chatfd[2];
  int status;

	if(pipe(fd) == -1) {
		perror("Failed to create the pipe");
		return 1;
	}
  if(pipe(chatfd) == -1) {
		perror("F
ailed to create the pipe");
		return 1;
	}
	pid = fork(); // create chatroom!

	if(pid == 0) { // chatroom
    chatroom(fd, chatfd, bufin, bufout);
/*

    close(chatfd[1]);
    close(fd[0]);
    for(int i = 0; i < 5; i++){
      write(fd[1], "Please respond with one of the following choices:\n 1) LOGIN <client_ID, password>\n 2) REGISTER <client_ID, password>\n 3) DISCONNECT\n\0", 512);
      read(chatfd[0], bufin, sizeof(bufin));
      fprintf(stderr, "Message from child is: %s\n", bufin);
    }
    fprintf(stderr, "Chatroom pid is %d\n", (int)getpid());
   */ return 1;

  }
  else { // parent
	  bytesin = strlen(bufin);
	
	  for(int i = 0; i < 5; i++) {
		  childpid = fork();
		  if(childpid == -1) {
			  perror("Failed to fork");
			  return 1;
		  }
      std::cout << "Forked!" << std::endl;
		  //if(childpid) // parent
			  //write(fd[1], bufout, strlen(bufout)+1);
		  if(childpid == 0) { // child
        close(chatfd[0]);
        close(fd[1]);
			  bytesin = read(fd[0], bufin, BUFSIZE);
        write(chatfd[1], "whats up chatroom!", 18);
		    fprintf(stderr, "Child [%ld]:my bufin is {%.*s}, my bufout is {%s}\n",
						  (long)getpid(), bytesin, bufin, bufout);
        return 1;
      }
      fprintf(stderr, "Parent [%ld]:my bufin is {%.*s}, my bufout is {%s}\nMy i is %d\n",
						  (long)getpid(), bytesin, bufin, bufout, i);
	  }
    while((wpid = wait(&status)) > 0) {
      printf("Exit status of %d was %d (%s)\n", (int)wpid, status,
            (status > 0) ? "accept" : "reject");
    }
  }
	return 0;
}
