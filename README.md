# ChatRoom
C/C++ Chatroom based on server-client model
Code must be compiled with -std=c++11 option
Not complete Object Oriented or pure C++ code

Current project progress may be viewed at: https://github.com/Rouvem/ChatRoom

How to compile:
1) Direct your terminal directory to the project directory
2) Run "make"

How to clean:
1) Remove the server and client executables: "make clean"
2) Remove the server and client executables along with all object files: "make superclean"

How to run server:
1) Type in the terminal "./server accounts.dat"
	1.1) If you wish to supply a new file for account management, simply enter in any filename you wish instead of "accounts.dat"

How to run client:
1) Ensure that the server is running
2) Type in the terminal "./client"

IMPORTANT NOTES:
- Running two clients from the same directory and transferring files via FPUT and FGET will result in the file being overwritten! This is problematic if you have a special extension such as .pdf or .mp3 or .docx etc.. as this will cause the file to be corrupted since it is not going to be in the correct format!

- Server and Client connections are by default assigned to port 1065. This may be changed by you the user by editing the code. Note that client-client connections use randomly generated port numbers with it being assigned "0" for the OS to figure it out.

- If you have issues compiling due to "execv" problems in the g++ version, please use a different computer system. Thank you!

- The folder "files" is simply a directory for placing files to be transferred from client to client.

- The folder "prototypes" contains files that were used during project design and were simply tests. You may ignore this.

- 
