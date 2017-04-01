#include "server.h"
#include <iostream>
using namespace std;

int main() {
  Server server;
  server.Setup("hi.dat");
  server.Scan();
  return 0;
}
