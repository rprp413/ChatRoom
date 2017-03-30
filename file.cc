#include "file.h"
#include <iostream>
#include <fstream>
#include <string>

bool File::OpenIn(void) {
  inFile.open(fileName, ios::app | ios::ate);
  if(inFile.is_open()) {
    return true;
  }
  else {
    return false;
  }
}

bool File::OpenOut(void) {
  outFile.open(fileName, ios::app | ios::ate);
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
