all: run_server

CC = g++
CFLAGS = -std=c++11

main: main_server main_client

main_server: main_server.cc
	$(CC) $(CFLAGS) -o ser main_server.cc -lpthread

main_client: main_client.cc
	$(CC) $(CFLAGS) -o cli main_client.cc -lpthread

run_server: chatroom.o server.o file.o run_server.cc
	$(CC) $(CFLAGS) -pthread -o run_server run_server.cc server.o chatroom.o file.o

run_client: client.o run_client.cc
	$(CC) $(CFLAGS) -o run_client run_client.cc client.o

client.o: client.cc client.h
	$(CC) $(CFLAGS) -c -o client.o client.cc

chatroom.o: chatroom.cc
	$(CC) $(CFLAGS) -c -o chatroom.o chatroom.cc

file.o: file.cc
	$(CC) $(CFLAGS) -c -o file.o file.cc

server.o: server.cc server.h
	$(CC) $(CFLAGS) -pthread -c -o server.o server.cc

clean:
	rm run_server

superclean:
	rm *.o run_server
