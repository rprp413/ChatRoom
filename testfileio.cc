#include <iostream>
#include <fstream>
#include <string>
#include "file.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

using namespace std;

int main() {

  string input, word;
  getline(cin, input);
  stringstream ss(input);
  vector<string> s;
  
  while(getline(ss, word, ' ')) {
    s.push_back(word);
  }


  char *text[100];
  

  //s.push_back(input);
  for(vector<string>::iterator iter = s.begin(); iter != s.end(); iter++) {
    cout << *iter << endl;
  }
  cout << input << endl;
  File f("accounts.dat");
  f.OpenIn();
  f.OpenOut();

  if((*(s.begin()) == "LOGIN") && (s.size() == 3)) {
    cout << "logging in" << endl;
    f.ReadFile(*(s.begin()+1), *(s.begin()+2));
  }
  if((*(s.begin()) == "REGISTER") && (s.size() == 3)) {
    cout << "registering" << endl;
    f.WriteFile(*(s.begin()+1), *(s.begin()+2));
  }
  if(s.size() != 3) {
    cout << "Input must be three items!" << endl << "LOGIN <client_ID, password>" << endl;
  }
  f.CloseFile();

  return 0;
}
