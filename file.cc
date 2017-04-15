#include "file.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

File::File() {}

File::File(string file_name) : fileName(file_name) {}

void File::SetFileName(string file_name) {
  fileName = file_name;
}

bool File::OpenIn(void) {
  inFile.open(fileName.c_str(), ios::app);
  if(inFile.is_open()) {
    return true;
  }
  else {
    return false;
  }
}

bool File::OpenOut(void) {
  outFile.open(fileName.c_str(), ios::app | ios::ate);
  if(outFile.is_open()) {
    return true;
  }
  else {
    return false;
  }
}

void File::CloseFile(void) {
  if(inFile.is_open()) {
    inFile.close();
  }
  if(outFile.is_open()) {
    outFile.close();
  }
}

bool File::CheckID(string client_ID) {
  string temp_client_ID, temp_password;
  
  while(inFile >> temp_client_ID >> temp_password) {
    cout << temp_client_ID << " " << temp_password << endl;
    if((temp_client_ID == client_ID)) {
      // cout << "ID Exists" << endl;
      return 1;
    }
  }
  return 0; // ID DNE
}

bool File::ReadFile(string client_ID, string password) { // AKA CheckPassword!
  string temp_client_ID, temp_password;
	cout << "input is: " << client_ID << " " << password << endl;
  while(inFile >> temp_client_ID >> temp_password) {
    // cout << temp_client_ID << " " << temp_password << endl;
    if((temp_client_ID == client_ID) && (temp_password == password)) {
      cout << "Login Successful!" << endl;
      return 1;
    }
  }
  // cout << "Login Unsuccessful! Wrong ID and/or password!" << endl;
  return 0;
}

bool File::WriteFile(string client_ID, string password) {
  string temp_client_ID, temp_password;
  if(ReadFile(client_ID, password) == 1) {
    //cout << "Account already exists, Logging in!" << endl;
    return 0;
  }
  else {
    outFile << client_ID << " " << password << endl;
    //cout << "Account newly created!" << endl;
  }
  return 1;
}
