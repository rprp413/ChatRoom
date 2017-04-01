#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;
struct Data{
  int x;
  char a;
};

struct ClientInfo {
  string client_ID;
  string password;
};

int main() {
  vector<Data> data;
  Data some1 = {10,'c'};
  Data some2 = {20, 'r'};
  Data some3 = {30, 'z'};

  data.push_back(some1);
  data.push_back(some2);
  data.push_back(some3);

  vector<ClientInfo> clients;
  
  ClientInfo temp_client;

  char msg[256] = {"Rouvem jalioldlife21"};
  
  string parse;
  istringstream iss(msg);

  int i = 0;  
  char *temp = strtok (msg, " ");

  vector<string> s;
  while(temp != NULL) {
    s.push_back(string(temp));
    printf("%s\n", temp);
    temp = strtok(NULL, " ");
  }
  temp_client.client_ID = *(s.begin());
  temp_client.password = *(s.begin()+1);
  cout << "here" << temp_client.client_ID << " " << temp_client.password << endl;


  for(vector<Data>::iterator iter = data.begin(); iter != data.end(); iter++) {
    cout << (*iter).x << endl;
  }
return 0;
}
