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

To transfer files with "FPUT" and "FGET":
1) The client that wishes to FPUT must enter their ip address and port number
	1.1) The ip address and port number are given upon running "./client"
	1.2) If the ip address is shown as "0", this means you have to enter "0.0.0.0" followed by the port number
2) The client that wishes to FGET need only check FLIST and see a file for transfer
3) Do not FPUT a file from a different folder, as this will cause problems with inexistent folders on the recipients end and cause SegFaults!

IMPORTANT NOTES:
- Running two clients from the same directory and transferring files via FPUT and FGET will result in the file being overwritten! This is problematic if you have a special extension such as .pdf or .mp3 or .docx etc.. as this will cause the file to be corrupted since it is not going to be in the correct format!

- Server and Client connections are by default assigned to port 1065. This may be changed by you the user by editing the code. Note that client-client connections use randomly generated port numbers with it being assigned "0" for the OS to figure it out.

- The folder "prototypes" contains files that were used during project design and were simply tests. You may ignore this.

- The given file "a.txt" is a test file for inputting and outputting files
