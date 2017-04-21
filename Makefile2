CC = g++
CFLAGS = -std=c++11 -Wall -lpthread -pthread #is compiler flags
CCFLAGS = -Iinclude # -I is a preprocessor flag, not a compiler flag
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin
SERSRC = src/server.cc src/chatroom.cc src/file.cc
CLISRC = src/client.cc src/file.cc
INC = $(wildcard $(INC_DIR)/*.h)
SEROBJ = $(SERSRC:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)
CLIOBJ = $(CLISRC:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)
EXE = bin/server bin/client
all: $(EXE)

bin/server: src/run_server.cc $(SEROBJ) | bin
	$(CC) $(CFLAGS) $(CCFLAGS) $^ -o $@

bin/client: src/run_client.cc $(CLIOBJ) | bin
	$(CC) $(CFLAGS) $(CCFLAGS) $^ -o $@

obj/%.o: src/%.cc include/%.h include/codes.h | obj
	$(CC) $(CCFLAGS) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.cc | obj
	$(CC) $(CCFLAGS) $(CFLAGS) -c $< -o $@

bin obj:
	mkdir -p $@

clean:
	$(RM) -r $(EXE) bin

superclean:
	$(RM) -r $(EXE) $(SEROBJ) bin obj
