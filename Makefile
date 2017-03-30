all: server client

server: main_server.cc
	g++ -o server main_server.cc

client: main_client.cc
	g++ -o client main_client.cc

clean:
	rm server client
