/*
* @filename sender.c
* @author Alex Lindemann , Nathan Moore
* @created 03/04/2017
* @desc This is a sender program implementing rdt 3.0
* sender port rcvHost rcvPort networkHost networkPort
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "rdtSender.h"
#include "definitions.h"

void getMessageFromUser(char *);
void checkCommandLineArguments(int, char**);
int setup_network (int* , char *, int , char *, int , struct hostent** , struct sockaddr_in * , struct sockaddr_in * , int);
int main (int argc, char *argv[]) {
  checkCommandLineArguments(argc, argv);
  char messageToSend[500];
  getMessageFromUser(messageToSend);
  sendMessage(atoi(argv[1]), argv[4], atoi(argv[5]), argv[2], atoi(argv[3]), messageToSend);

  //setup_network(argv, &sockfd, &hostptr, &network, &src, &ME_PORT, &DEST_PORT, &NETWORK_PORT);

  return 0;
}

void checkCommandLineArguments(int argc, char **argv) {
  if (argc != 6) {
    fprintf(stderr, "Usage: sender port rcvHost rcvPort networkHost networkPort\n");
    exit(1);
  }
}

int setup_network(int* sockfd, char *netwhost, int netwPort, char *desthost, int destPort, struct hostent** hostptr, struct sockaddr_in * network, struct sockaddr_in * src, int localPort){
*sockfd = socket(AF_INET, SOCK_DGRAM, 0);
 if (sockfd < 0 ) {
   perror("error opening socket");
   return -2;
 }
 *hostptr = gethostbyname(netwhost);
 memset((void *) network, 0, (size_t) sizeof(*network));
 network->sin_family = AF_INET;
 memcpy((void *) &(network->sin_addr), (void *) (*hostptr)->h_addr, (*hostptr)->h_length);
 network->sin_port = htons((u_short) netwPort);

 memset((void *) src, 0, sizeof(*src));
 src->sin_family = AF_INET;
 src->sin_addr.s_addr = htonl(INADDR_ANY);
 src->sin_port = htons(localPort);
 if(bind (*sockfd, (struct sockaddr *) src, sizeof(*src)) < 0) {
   perror("bind");
   return -1;
 }
 fprintf(stderr, "\nWe are listening on: %s:%d\n", inet_ntoa(src->sin_addr), ntohs(src->sin_port));
 fprintf(stderr, "\nWe are sending to the network: %s:%d\n", inet_ntoa(network->sin_addr), ntohs(network->sin_port));
return 0;
}

/*
* Prompts the user for a string
* @params, char* buffer - where the string will be stored
*/
void getMessageFromUser(char *buffer) {
  printf("Enter the message to send: ");
  fgets(buffer, 500, stdin);
  return;
}

int sendMessage (int localPort, char* netwhost, int netwPort, char* desthost, int destPort, char* message) {
  int sockfd;
  struct hostent *hostptr;
  struct sockaddr_in network, src;
  int err;
  if ((err = setup_network(&sockfd, netwhost, netwPort, desthost, destPort, &hostptr, &network, &src, localPort)) < 0) {
    return err;
  }
  sendto(sockfd, message, PACKET_LENGTH, 0, (struct sockaddr *) &network, sizeof(network));


  return 0;
}
