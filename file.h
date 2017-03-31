#ifndef FILE_H_
#define FILE_H_

#include <fstream>
#include <string>
#include <iostream>

class File {
public:
  File(std::string file_name);
  void SetFileName(std::string file_name);
  bool OpenIn(void);
  bool OpenOut(void);
  void CloseFile(void);
  bool CheckID(std::string client_ID);
  bool ReadFile(std::string client_ID, std::string password);
  bool WriteFile(std::string client_ID, std::string password);
 // ios::app   -- Append to the file
 // ios::ate   -- Set the current position to the end
 // ios::trunc -- Delete everything in the file
private:
  std::string fileName;
  std::ifstream inFile;
  std::ofstream outFile;
};

#endif // FILE_H_
