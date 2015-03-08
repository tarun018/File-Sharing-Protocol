# File-Sharing-Protocol

###Compilation:
- $ chmod 777 run.sh
- $ ./run.sh

###Run Using:
- $ ./filetransferpro <ip of server> <client portno> <server portno> <protocol>

###Commands you can use:
- IndexGet --longlist
- IndexGet --shortlist starting-time-stamp ending-time-stamp   
- IndexGet --regEx  
- FileHash --verify Name-of-file
- FileHash --checkall
- FileDownload Name-of-file
- FileUpload Name-of-file

###Approach:
- Implemented TCP/Ip by making a connection between the two (using bind, listen , accept) and then using send and receive commands.
- Implemented UDP by just sending packets to the destination and using sendTo and recvFrom commands. Without any connection.
- Implemented Regex using regex.h

