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

// "OpenIn" means open for input
bool File::OpenIn(void) {
  inFile.open(fileName.c_str(), ios::app);
  if(inFile.is_open()) {
    return true;
  }
  else {
    return false;
  }
}

// "OpenOut" means open for output
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

// Checks if user is in the list of possible accounts
// Used for Registering and checking if account already exists
// AKA "client_ID already taken" or "duplicate client_ID"
// returning a 0 should result in a 0x02 error code
bool File::CheckID(string client_ID) {
  string temp_client_ID, temp_password;
  while(inFile >> temp_client_ID >> temp_password) {
    if((temp_client_ID == client_ID)) {
      return 1;
    }
  }
  return 0; // ID DNE
}

// Checks client_ID and password, used for logging in
// A return 0 correlates to an error code of 0x01
// That is an invalid client_ID and/or password
bool File::ReadFile(string client_ID, string password) {
  string temp_client_ID, temp_password;
  while(inFile >> temp_client_ID >> temp_password) {
    if((temp_client_ID == client_ID) && (temp_password == password)) {
      return 1;
    }
  }
  return 0;
}

// Used for Registration purposes where you want to add an account
bool File::WriteFile(string client_ID, string password) {
  string temp_client_ID, temp_password;
  if(ReadFile(client_ID, password) == 1) {
    return 0;
  }
  else {
    outFile << client_ID << " " << password << endl;
  }
  return 1;
}
