# TCP and UDP Client-Server-in-C
Small networking client and server project in C.

readFile to send info from the TCP server to client. Feel free to fork and play with the code. 

Usage :



  TCP:
    gcc GV_TCPServer.c -o GV_TCPServer
    gcc GV_TCPClient.c -o GV_TCPClient
    ./GV_TCPServer 1111
    ./GV_TCPClient localhost 1111    



 UDP:
    gcc GV_UDPServer.c -o GV_UDPServer
    gcc GV_UDPClient.c -o GV_UDPClient
    ./GV_UDPServer 1111
    ./GV_UDPClient localhost 1111
    
