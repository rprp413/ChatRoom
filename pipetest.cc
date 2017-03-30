#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define BUFSIZE 100

int main(void) {
	char bufin[BUFSIZE] = "empty";
	char bufout[] = "hello";
	int bytesin;
	pid_t childpid;
	int fd[2];

	if(pipe(fd) == -1) {
		perror("Failed to create the pipe");
		return 1;
	}

	bytesin = strlen(bufin);
	
	for(int i = 0; i < 2; i++) {
		childpid = fork();
		if(childpid == -1) {
			perror("Failed to fork");
			return 1;
		}
		if(childpid) // parent
			write(fd[1], bufout, strlen(bufout)+1);
		else // child
			bytesin = read(fd[0], bufin, BUFSIZE);
		fprintf(stderr, "[%ld]:my bufin is {%.*s}, my bufout is {%s}\n",
						(long)getpid(), bytesin, bufin, bufout);
	}
	return 0;
}
