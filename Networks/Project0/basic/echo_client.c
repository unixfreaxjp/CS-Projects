#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RCVBUFSIZE 32	//size of recieve buffer

void dieWithError(char* errorMessage); //error handling function

int main(int argc, char** argv)
{
	int sock;								//socket descrriptor
	struct sockaddr_in echoServerAddr;		//echo server address
	unsigned short echoServerPort;			//echo server port
	char* serverIP;							//server ip address (dotted quad)
	char* echoString;						//string to send to server
	char echoBuffer[RCVBUFSIZE];			//buffer for echo string
	unsigned int echoStringLen;				//length of string to echo
	int bytesRecieved, totalBytesRecieved;	//bytes read in single recv and total bytes read

	if(argc < 3 || argc > 4){				//test for correct arguments
		fprintf(stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n", argv[0]);
		exit(1);
	}

	serverIP = argv[1];
	echoString = argv[2];

	if(argc == 4){
		echoServerPort = atoi(argv[3]);		//use given port
	} else {
		echoServerPort = 7;					//7 is default
	}

	//create a reliable stream socket using TCP
	if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		dieWithError("socket() failed");
	}

	//construct server address structure
	memset(&echoServerAddr, 0, sizeof(echoServerAddr));		//zero out structure
	echoServerAddr.sin_family = AF_INET;					//internet address family
	echoServerAddr.sin_addr.s_addr = inet_addr(serverIP);	//server ip address
	echoServerAddr.sin_port = htons(echoServerPort);		//server port

	//establish connection to the echo server
	if(connect(sock, (struct sockaddr*)&echoServerAddr, sizeof(echoServerAddr)) < 0){
		dieWithError("connect() failed");
	}

	echoStringLen = strlen(echoString);		//determine input length

	//send the string to the server
	if(send(sock, echoString, echoStringLen, 0) != echoStringLen){
		dieWithError("send() sent a difference number of bytes than expected");
	}

	//recieve the same string back from the server
	totalBytesRecieved = 0;
	printf("Client recieved: ");	//set to print the echoed string

	while(totalBytesRecieved < echoStringLen){
		//recieve up to the buffer size (minus 1 to leave space for the null terminator) bytes from the sender
		if((bytesRecieved = recv(sock, echoBuffer, RCVBUFSIZE-1, 0)) <= 0){
			dieWithError("recv() failed for connection closed prematurely");
		}
		totalBytesRecieved += bytesRecieved;	//keep tally of total bytes
		echoBuffer[bytesRecieved] = '\0';		//terminate string
		printf("%s", echoBuffer);

	}

	printf("\n");	//print final linefeed
	close(sock);	//close socket
	exit(0);

}

void dieWithError(char* errorMessage)
{
	fprintf(stderr, "%s\n", errorMessage);
	exit(1);
}