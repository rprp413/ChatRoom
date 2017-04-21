all: server client

CC = g++
CFLAGS = -std=c++11

main: main_server main_client

main_server: main_server.cc
	$(CC) $(CFLAGS) -o ser main_server.cc -pthread

main_client: main_client.cc
	$(CC) $(CFLAGS) -o cli main_client.cc -pthread

server: chatroom.o server.o file.o run_server.cc
	$(CC) $(CFLAGS) -pthread -o server run_server.cc server.o chatroom.o file.o

client: client.o run_client.cc
	$(CC) $(CFLAGS) -pthread -o client run_client.cc client.o 

client.o: client.cc client.h
	$(CC) $(CFLAGS) -pthread -c -o client.o client.cc 

chatroom.o: chatroom.cc chatroom.h
	$(CC) $(CFLAGS) -c -o chatroom.o chatroom.cc

file.o: file.cc file.h
	$(CC) $(CFLAGS) -c -o file.o file.cc

server.o: server.cc server.h
	$(CC) $(CFLAGS) -pthread -c -o server.o server.cc

clean:
	rm -f run_server run_client

superclean:
	rm -f *.o run_server run_client
