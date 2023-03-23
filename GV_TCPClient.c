/**
 * @file GV_TCPClient.c
 * @date 2023-03-08
 * localhost 1111
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <time.h>
#define FILE_NAME_SIZE 1024
#define MAX_SIZE 1024

int main(int argc, char **argv) {
    char *hostname = argv[1];
    int portNum = atoi(argv[2]);
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char clientBuffer[FILE_NAME_SIZE];
	struct timespec start, end;
    if (argc != 3) {
        printf("TCP Client needs a hostname and port.\n");
        return -1;
    }

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0){
        printf("Couldn't create the socket.\n");
        return -1;  
    } 

    server = gethostbyname(hostname);
    if (server == NULL) {
        printf("Cound't find needed hostname.\n");
        return -1;
    }

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portNum);
    if(connect(clientSocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0){
        printf("Couldn't connect to server\n");
        return -1;
    } 
      
    printf("Enter the file you want read from the server - use readFile.txt: \n");
    memset(clientBuffer, 0, MAX_SIZE);
    fgets(clientBuffer, FILE_NAME_SIZE, stdin);

    //send the file name to the server
    if(write(clientSocket, clientBuffer, strlen(clientBuffer)) < 0){
      printf("Couldn't write to server.\n");
      return -1;
    } 
    bzero(clientBuffer, FILE_NAME_SIZE);
    //read and print the server's response
    int count = 1;
    FILE *newPtr;
    //a+ mode lets make a file or edit one already created
    newPtr = fopen("newFile.txt", "a+");
    if(newPtr == NULL){
        printf("Couldn't create a new file.\n");
        return -1;
    }
    while(read(clientSocket, clientBuffer, sizeof clientBuffer) > 0){
      fprintf(newPtr, "%s", clientBuffer);
      bzero(clientBuffer, sizeof clientBuffer);
    }
    fclose(newPtr);
    close(clientSocket);
    return 0;
}
