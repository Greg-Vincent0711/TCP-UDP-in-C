/**
 * @file GV_TCPServer.c
 * @author Gregory Vincent
 * @date 2023-08-03
 * 1111
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_SIZE 1024
#define MAX_LISTEN 5
//the line length fgets is going to read
#define LINE_LENGTH 80 

void addNullTerm(char *arg);

//add a null termninator to the string
void addNullTerm(char *arg){
  //find the first occurence of the newline
    char *findNewLine = strchr(arg, '\n');
    if (findNewLine != NULL){
        //set it to the null terminator so that 
        *findNewLine = '\0';
    }
}

int main(int argc, char **argv) {
  int childSocket; 
  socklen_t clientSize;
  struct sockaddr_in serveraddr; 
  struct sockaddr_in clientaddr; 
  struct hostent *clientHostInfo; 
  char serverBuffer[MAX_SIZE];
  char *hostaddrp;

  if(argc != 2){
    printf("TCP Server needs a port.\n");
    return -1;
  }
  int portNum = atoi(argv[1]);
  int parentSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(parentSocket < 0){
    printf("Couldn't create the socket.\n");
    return -1;
  } 
  int optval = 1;
  setsockopt(parentSocket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portNum);
  if(bind(parentSocket, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0){
    printf("Couldn't bind port to socket.\n");
    return -1;
  } 

  if(listen(parentSocket, MAX_LISTEN) < 0){
    printf("Error listening to incoming connections.\n");
    return -1;
  } 
  clientSize = sizeof(clientaddr);
  printf("Attempting to establish a connection...\n");
  while(1){
    childSocket = accept(parentSocket, (struct sockaddr *) &clientaddr, &clientSize);
    if(childSocket < 0){
      printf("Couldn't accept connections.\n");
      return -1;    
    } 
    clientHostInfo = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    if(clientHostInfo == NULL){
      printf("Error getting proper address information.\n");
      return -1;
    }
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if(hostaddrp == NULL){
      printf("Error getting client address.\n");
      return -1;
    }
    if(read(childSocket, serverBuffer, MAX_SIZE) < 0){
      printf("Error reading from the client.\n");
      return -1;
    }

    addNullTerm(serverBuffer);
    //read through existing file
    FILE *fPtr = fopen(serverBuffer, "r"); 
    if(fPtr == NULL){
        printf("Couldn't open file.\n");
        return -1;
    }
    char tempBuffer[MAX_SIZE];
    //send contents line by line
    while(fgets(tempBuffer, LINE_LENGTH, fPtr) != NULL){
      write(childSocket, tempBuffer, strlen(tempBuffer));
      bzero(tempBuffer, sizeof tempBuffer);
    }
    //send the special character if we reach EOF
    if(feof(fPtr)){
      bzero(tempBuffer, sizeof tempBuffer);
      write(childSocket, "$", sizeof "$");
    }
    printf("Sent all contents to the file.\n");
    fclose(fPtr);
    close(childSocket);
  }
  close(parentSocket);
  return 0;
} 