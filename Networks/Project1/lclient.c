#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define RECV_SIZE 1024
#define PORT 14456


void dieWithError(char* message);
unsigned long resolveName(char* name);
void sendToServer(int sock, char* message);

unsigned int isOpen = 0;
unsigned int quit = 0;
int main(int argc, char** argv)
{

	int sock;
	struct sockaddr_in serverAddr;
	char* serverIP;

	FILE* istream;
	char* line = NULL;
	size_t lineSize = 0;
	int lineCount = 0;

	//make sure argument format is correct
	if(argc < 2 || argc > 3){
		fprintf(stderr, "Usage: %s <server> [inputFile]\n", argv[0]);
		exit(1);
	}

	serverIP = argv[1];	//get server name

	//pick the input source, file or stdin
	if(argc == 3){
		istream = fopen(argv[2], "r");
	} else {
		istream = stdin;
	}

	while(quit == 0){
		if(isOpen == 0){
			//printf("trying to open another connection\n");
			//sleep(1);

			if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){	//create tcp socket
				dieWithError("socket failed()");
			}

			//construct server address structure
			memset(&serverAddr, 0, sizeof(serverAddr));		//zero out structure
			serverAddr.sin_family = AF_INET;					//internet address family
			serverAddr.sin_addr.s_addr = resolveName(serverIP);	//server ip address
			serverAddr.sin_port = htons(PORT);		//server port

			if(connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0){
				dieWithError("connect() failed");
			}
			isOpen = 1;
		}
		//printf("looping\n");
		while(isOpen){
			lineCount = getline(&line, &lineSize, istream);
			line[lineCount-1] = '\0';
			sendToServer(sock, line);
		}
		//printf("closing\n");
		close(sock);

	}
	//printf("quitting\n");

	return 0;
}

void sendToServer(int sock, char* message)
{
	int mlen = strlen(message);
	int rlen = 0;
	char buffer[RECV_SIZE];


	if(send(sock, message, mlen, 0) != mlen){
		dieWithError("send() sent a different number of bytes than expected");
	}

	if((rlen = recv(sock, buffer, RECV_SIZE, 0)) < 0){
		dieWithError("recv() failed");
	}

	buffer[rlen] = '\0';

	//check if the server is closing the connection or quitting
	if(strstr(buffer, "CLOSING") == buffer){
		isOpen = 0;
	}

	if(strstr(buffer, "QUITTING") == buffer){
		isOpen = 0;
		quit = 1;
	}

	// if(strstr(buffer, "LIST") == buffer){

	// 	while(1){
	// 		printf("waiting list\n");
	// 		if((rlen = recv(sock, buffer, RECV_SIZE, 0)) < 0){
	// 			dieWithError("recv() failed");
	// 		}
	// 		printf("got\n");
	// 		buffer[rlen] = '\0';
	// 		if(strcmp(buffer, "ENDLIST") == 0){
	// 			break;
	// 		}

	// 		printf("SERVER list-> %s\n", buffer);

	// 	}
	// }

	printf("SERVER %s\n", buffer);

}

unsigned long resolveName(char* name)
{
	struct hostent *host;

	 if((host = gethostbyname(name)) == NULL){
	 	dieWithError("gethostbyname() failed");
	 }

	 return *((unsigned long *)host->h_addr_list[0]);
}

void dieWithError(char* message)
{
	fprintf(stderr, "%s\n", message);
	exit(1);
}