all: run_server

CC = g++
CFLAGS = -std=c++11

run_server: chatroom.o server.o file.o run_server.cc
	$(CC) $(CFLAGS) -pthread -o run_server run_server.cc server.o chatroom.o file.o

client: main_client.cc
	$(CC) $(CFLAGS) -o client main_client.cc

chatroom.o: chatroom.cc
	$(CC) $(CFLAGS) -c -o chatroom.o chatroom.cc

file.o: file.cc
	$(CC) $(CFLAGS) -c -o file.o file.cc

server.o: server.cc
	$(CC) $(CFLAGS) -pthread -c -o server.o server.cc

clean:
	rm run_server

superclean:
	rm *.o run_server
