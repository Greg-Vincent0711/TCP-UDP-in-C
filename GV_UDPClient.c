/**
 * @file GV_UDPClient.c
 * @author Gregory Vincent
 * @date 2023-02-25
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
#define MSG_SIZE 1024
//a 1012 character can be sent excluding the header's length
#define USR_MSG_SIZE 1012
void stripHeader(char *originalStr, const char *subStr);

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

int main(int argc, char **argv) {
    socklen_t serverlen;
    struct sockaddr_in serverAddress;
    struct hostent *serverHost;
    char *hostname;
    char msgPipe[MSG_SIZE] = "UDP CLIENT 1 ";
    char userMessage[USR_MSG_SIZE];

    if(argc != 3) {
       printf("A host and port are needed.\n");
       return -1;
    }

    hostname = argv[1];
    int portno = atoi(argv[2]);

    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udpSocket < 0) {
        printf("Couldn't create the socket.\n");
        return -1;
    }

    serverHost = gethostbyname(hostname);
    if(serverHost == NULL) {
        printf("Couldn't find host name.\n");
        return -1;
    }
    
    /*Create the server address struct*/
    bzero((char *) &serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    bcopy((char *)serverHost->h_addr, (char *)&serverAddress.sin_addr.s_addr, serverHost->h_length);
    serverAddress.sin_port = htons(portno);

    /* get a message from the user and add the header */
    printf("Enter a message for the server:  \n");
    fgets(userMessage, USR_MSG_SIZE, stdin);
    strcat(msgPipe, userMessage);

    serverlen = (socklen_t) sizeof(serverAddress);
    if(sendto(udpSocket, msgPipe, strlen(msgPipe), 0, (struct sockaddr*)&serverAddress, serverlen) < 0){
      printf("Error sending the message to server.\n");
      return -1;
    }
    if(recvfrom(udpSocket, msgPipe, strlen(msgPipe), 0, (struct sockaddr*)&serverAddress, &serverlen) < 0){
      printf("Error recieving the message from server.\n");
      return -1;
    }
    stripHeader(msgPipe, "UDP SERVER");
    printf("Server response with header stripped: %s\n", msgPipe);
        
    return 0;
}