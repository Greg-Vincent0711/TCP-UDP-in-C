/**
 * @file GV_UDPServer.c
 * @author Gregory Vincent
 * @date 2023-02-25
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
#include <ctype.h>
#define MSG_SIZE 1024
char userMsg[MSG_SIZE];
char sendMsg[MSG_SIZE] = "UDP SERVER ";
void stripHeader(char *originalStr, const char *subStr);
void makeUpper(char *arg);


//helper function that strips substrings
void stripHeader(char *originalStr, const char *subStr){
  int removeLength = strlen(subStr);
  char *ref = originalStr;
  //if we find the substring
  while(strstr(originalStr, subStr)){
    //move all other portions of the string to overwrite it's space
    memmove(ref, ref + removeLength, strlen(ref + removeLength) + 1);
  }
}

//helper function to make strings uppercase
void makeUpper(char *arg){
  for(int i = 0; i < strlen(arg); i++){
    arg[i] = toupper(arg[i]);
  }
}

int main(int argc, char **argv) {
  socklen_t clientlen; 
  struct sockaddr_in serverAddress; 
  struct sockaddr_in clientAddress;
  /* client host info */ 
  struct hostent *clientHostInfo; 
  /* host address string */
  char *hostaddrp; 
  int msgSize; 

  if(argc != 2) {
    printf("A port number is needed.\n");
    return -1;
  }
  int portNumber = atoi(argv[1]);
  int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
  if(udpSocket < 0) {
    printf("Couldn't successfully create a socket.\n");
    return -1;
  }

  int optval = 1;
  setsockopt(udpSocket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

  bzero((char *) &serverAddress, sizeof(serverAddress));
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddress.sin_port = htons((unsigned short)portNumber);

  
  if(bind(udpSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
    printf("Couldn't successfully bind to a port.\n");
    return -1;
  } 
    

  clientlen = (socklen_t) sizeof(clientAddress);
  printf("Listening for messages.\n");
  while(1){
    // bzero(userMsg, MSG_SIZE);
    if((recvfrom(udpSocket, userMsg, MSG_SIZE, 0,(struct sockaddr *) &clientAddress, &clientlen)) < 0){
      printf("Recieving error.\n");
      return -1;
    }

    clientHostInfo = gethostbyaddr((const char *)&clientAddress.sin_addr.s_addr, sizeof(clientAddress.sin_addr.s_addr), AF_INET);
    if(clientHostInfo == NULL){
      printf("Couldn't find information sender.\n");
      return -1;
    }
    hostaddrp = inet_ntoa(clientAddress.sin_addr);
    if(hostaddrp == NULL){
      printf("Host address error.\n");
      return -1;
    }

    //keep track of message pre and post strip
    printf("Message from client: %s\n", userMsg);
    stripHeader(userMsg, "UDP CLIENT 1 ");
    makeUpper(userMsg);
    printf("No header and uppercased: %s\n", userMsg);
    strcat(sendMsg, userMsg);
    printf("Message that will be sent to the client: %s\n", sendMsg);

    if((sendto(udpSocket, sendMsg, strlen(sendMsg), 0, (struct sockaddr *) &clientAddress, clientlen)) < 0){
      printf("Couldn't send.\n");
      return -1;
    }
    //reset msg buffers, reappend the header
    memset(userMsg, '\0', MSG_SIZE);
    memset(sendMsg, '\0', MSG_SIZE);
    strcat(sendMsg, "UDP SERVER ");
  }
  close(udpSocket);
  return 0;
}
