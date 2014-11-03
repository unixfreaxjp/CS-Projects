#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define RCVBUFSIZE 32	//size of recieve buffer
#define PORT 14456		//port to use
#define DLE_ETX "\x10\03"
void dieWithError(char* errorMessage); //error handling function
unsigned long resolveName(char* name); //maps from name to address
void sendMessage(int sock, char* message); //send and get echo from server
int main(int argc, char** argv)
{
	char dle_etx[] = "\x10\03";
	int sock;								//socket descrriptor
	struct sockaddr_in echoServerAddr;		//echo server address
	unsigned short echoServerPort;			//echo server port
	char* serverIP;							//server ip address (dotted quad)
	char echoBuffer[RCVBUFSIZE];			//buffer for echo string
	unsigned int echoStringLen;				//length of string to echo
	int bytesRecieved, totalBytesRecieved;	//bytes read in single recv and total bytes read
	int messagesSent, messages;

	if(argc < 3 || argc > 7){				//test for correct arguments
		fprintf(stderr, "Usage: %s <Server IP> <string 1> [string 2] [string 3] [string 4] [string 5]\n", argv[0]);
		exit(1);
	}
	messages = argc - 2;
	serverIP = argv[1];

	echoServerPort = PORT;

	//create a reliable stream socket using TCP
	if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		dieWithError("socket() failed");
	}

	//construct server address structure
	memset(&echoServerAddr, 0, sizeof(echoServerAddr));		//zero out structure
	echoServerAddr.sin_family = AF_INET;					//internet address family
	echoServerAddr.sin_addr.s_addr = resolveName(serverIP);	//server ip address
	echoServerAddr.sin_port = htons(echoServerPort);		//server port

	//establish connection to the echo server
	if(connect(sock, (struct sockaddr*)&echoServerAddr, sizeof(echoServerAddr)) < 0){
		dieWithError("connect() failed");
	}

	//TODO: echo all messages and dle etx
	messagesSent = 0;
	do {
		//printf("sending %s\n",argv[2+messagesSent] );
		sendMessage(sock, argv[2+messagesSent]);
		messagesSent++;
	} while(messagesSent < messages);

	sendMessage(sock, dle_etx);

	close(sock);	//close socket
	exit(0);

}

void sendMessage(int sock, char* message)
{
	int mlen = strlen(message);

	if(mlen > 12){
		printf("The string \"%s\" is longer than 12 bytes\n", message);
		return;
	}

	int totalBytesRecieved, bytesRecieved;
	char buffer[RCVBUFSIZE];
	if(send(sock, message, mlen, 0) != mlen){
		dieWithError("send() sent a different number of bytes than expected");
	}

	totalBytesRecieved = 0;


//	printf("Recieved: ");	//set to print the echoed string
	while(totalBytesRecieved < mlen){
		//recieve up to the buffer size (minus 1 to leave space for the null terminator) bytes from the sender
		if((bytesRecieved = recv(sock, buffer, RCVBUFSIZE-1, 0)) <= 0){
			dieWithError("recv() failed for connection closed prematurely");
		}
		totalBytesRecieved += bytesRecieved;	//keep tally of total bytes
		buffer[bytesRecieved] = '\0';		//terminate string

		if(strcmp(DLE_ETX, buffer) == 0){
			printf("EnhancedClient done");
		} else {

		printf("EnhancedClient recieved: %s\n", buffer);
	}

	}
	//printf("\n");


}

unsigned long resolveName(char* name)
{
	struct hostent *host;

	if((host = gethostbyname(name)) == NULL){
		dieWithError("gethostbyname() failed");
	}

	return *((unsigned long *) host->h_addr_list[0]);

}

void dieWithError(char* errorMessage)
{
	fprintf(stderr, "%s\n", errorMessage);
	exit(1);
}