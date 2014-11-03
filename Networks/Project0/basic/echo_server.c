#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXPENDING 5
#define RECVBUFSIZE 32

void dieWithError(char* errorMessage); //error handling function
void handleTCPClient(int clientSocket);  //TCP client handling function

int main(int argc, char** argv)
{
	int serverSocket;	//Socket descriptor for server
	int clientSocket;	//Socket descriptor for client

	struct sockaddr_in echoServerAddr;	//local address
	struct sockaddr_in echoClientAddr;	//client address

	unsigned short echoServerPort;	//server port
	unsigned int clientLen;			//length of client address data structure

	if(argc != 2){
		fprintf(stderr, "Usage %s <Server Port>\n", argv[0]);
		exit(1);
	}

	echoServerPort = atoi(argv[1]);   //first argument: local port

	//create socket for incoming connections
	if((serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		dieWithError("socket() failed");
	}

	//construct local address structure
	memset(&echoServerAddr, 0, sizeof(echoServerAddr));	//zero out structure
	echoServerAddr.sin_family = AF_INET;					//internet address family
	echoServerAddr.sin_addr.s_addr = htonl(INADDR_ANY); //any incoming interface
	echoServerAddr.sin_port = htons(echoServerPort);	//local port

	//bind to the local address
	if(bind(serverSocket, (struct sockaddr*)&echoServerAddr, sizeof(echoServerAddr)) < 0){
		dieWithError("bind() failed");
	}

	//mark the socket so it will listen for incoming connections
	if(listen(serverSocket, MAXPENDING) < 0){
		dieWithError("listen() failed");
	}

	//set the size of the in-out parameter
	clientLen = sizeof(echoClientAddr);

	//wait for client to connect
	if((clientSocket = accept(serverSocket, (struct sockaddr*)&echoClientAddr, &clientLen)) < 0){
		dieWithError("accept() failed");
	}

	//clientSocket is connected to a client
	printf("Handling client %s\n", inet_ntoa(echoClientAddr.sin_addr));
	handleTCPClient(clientSocket);
	
}

void handleTCPClient(int clientSocket)
{
	char echoBuffer[RECVBUFSIZE];
	int msgSize;

	//recieve message from client
	if((msgSize = recv(clientSocket, echoBuffer, RECVBUFSIZE, 0)) < 0){
		dieWithError("recv() failed");
	}

	//echo message back to client
	if(send(clientSocket, echoBuffer, msgSize, 0) != msgSize){
		dieWithError("send() failed");
	}

	recv(clientSocket, echoBuffer, RECVBUFSIZE, 0);

	close(clientSocket);


}

void dieWithError(char* errorMessage)
{
	fprintf(stderr, "%s\n", errorMessage);
	exit(1);
}
