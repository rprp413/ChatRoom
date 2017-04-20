#include "server.h"
#include <iostream>
using namespace std;

int main(int argc, char **argv) {
	if(argc != 2) {
		cout << "Format: ./server filename" << endl;
		return 0;
	}
  Server server;
  server.Setup(argv[1]);
  server.Scan();
  return 0;
}
