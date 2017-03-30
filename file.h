#ifndef FILE_H_
#define FILE_H_

#include <fstream>
#include <string>
#include <iostream>

class File {
public:

  bool OpenIn(void);
  bool OpenOut(void);
  void CloseFile(void);
 // ios::app   -- Append to the file
 // ios::ate   -- Set the current position to the end
 // ios::trunc -- Delete everything in the file
private:
  string fileName;
  ifstream inFile;
  ofstream outFile;
};

#endif // FILE_H_
