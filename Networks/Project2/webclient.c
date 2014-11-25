#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/tcp.h>

#define RECV_SIZE 10000

void dieWithError(char* message);
unsigned long resolveName(char* name);
int millis(struct timeval* t);

int main(int argc, char** argv)
{
	int sock;
	struct sockaddr_in serverAddr;
	
	char* url;
	char* host;
	char* path;
	int port = 80;

	if(argc == 1){
		fprintf(stderr, "Usage: %s <url> [-f|-nf|-ping|-pkt|-info] [file]\n", argv[0]);
		return 1;
	}

	url = argv[1];	//get server name

	//remove the http:// if it is present
	if(strstr(url, "http://") == url){
		url = url+7;
	}

	//get path
	path = strchr(url, '/');
	if(path == NULL){
		path = "/";
	}
	//get host
	host = malloc(path-url+1);
	strncpy(host, url, path-url);
	host[path-url] = '\0';

	//get port
	if(strchr(host, ':') != NULL){
		port = atoi(strchr(host, ':')+1);
		*strchr(host, ':') = '\0';
	}

	int noFile = 0;
	int ping = 0;
	int pkt = 0;
	int info = 0;
	char* fileName = "web.txt";

	if(argc > 2){
		if(strcmp(argv[2], "-f") == 0 && argc > 3){
			fileName = argv[3];
		} else if (strcmp(argv[2], "-nf")   == 0){
			noFile = 1;
		} else if (strcmp(argv[2], "-ping") == 0){
			ping = 1;
		} else if (strcmp(argv[2], "-pkt")  == 0){
			pkt = 1;
			noFile = 1;
		} else if (strcmp(argv[2], "-info") == 0){
			info = 1;
		}
	}

	//printf("HOST = %s\nPATH = %s\nPORT = %d\n", host, path, port);

	if((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){	//create tcp socket
		dieWithError("socket failed()");
	}

	//construct server address structure
	memset(&serverAddr, 0, sizeof(serverAddr));		//zero out structure
	serverAddr.sin_family = AF_INET;					//internet address family
	serverAddr.sin_addr.s_addr = resolveName(host);	//server ip address
	serverAddr.sin_port = htons(port);		//server port

	//time the RTT during connect()
	struct timeval start, end;
	gettimeofday(&start, NULL);
	if(connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0){
		dieWithError("connect() failed");
	}
	gettimeofday(&end, NULL);

	if(ping){
		//print timed RTT
		int timeMillis = millis(&end)-(start.tv_sec*1000 + start.tv_usec/1000);
		printf("%d\n", timeMillis);
	}


	//build request string message
	int packetTime[10000];
	int packetSize[10000];
	int packetCount = 0;
	char message[4096];

	//build HTTP request
	sprintf(message, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", path, host);

	//printf("[%s][[%s]]\n", path,message);

	int mlen = strlen(message);
	int rlen = 0;
	
	int totalSize = 0;
	char* responseMessage = NULL;
	char* buffer = malloc(RECV_SIZE);

	//send request to server
	if(send(sock, message, mlen, 0) != mlen){
		dieWithError("send() sent a different number of bytes than expected");
	}

	rlen = 1;

	gettimeofday(&start, NULL);

	//read the whole stream, time each packet
	while((rlen = recv(sock, buffer, RECV_SIZE, 0)) > 0){
		gettimeofday(&end, NULL);
		packetTime[packetCount] = millis(&end)-millis(&start);
		packetSize[packetCount] = rlen;
		packetCount++;

		responseMessage = realloc(responseMessage, totalSize + rlen);
		memcpy(responseMessage+totalSize, buffer, rlen);
		totalSize += rlen;

		gettimeofday(&start, NULL);
	}


	//print out packet timings
	if(pkt){
		int i;
		for(i = 0; i < packetCount; i++){
			printf("%d %d\n", packetTime[i], packetSize[i]);
		}
	}

	if(totalSize == 0){
		responseMessage[0] = ' ';
		totalSize++;
	}

	responseMessage[totalSize] = '\0';
	//printf("%s\n", responseMessage);
	//close the connection

	//write to file
	FILE* fp = NULL;

	if(noFile == 0){
		fp = fopen(fileName, "a");
		if(fp == NULL){
			dieWithError("Failed to open file");
		}
		fprintf(fp, "%s", responseMessage);
		fclose(fp);
	}

	//print socket RTT & RTT variance
	if(info){
		struct tcp_info tinfo;
		int optlen = sizeof(struct tcp_info);
		getsockopt(sock, IPPROTO_TCP, TCP_INFO, &tinfo, &optlen);
		int sockRTT = tinfo.tcpi_rtt/1000;
		int sockRTT_var = tinfo.tcpi_rttvar/1000;
		printf("RTT = %d\nRTT variance = %d\n", sockRTT, sockRTT_var);
	}


	close(sock);

	return 0;
}

unsigned long resolveName(char* name)
{
	struct hostent *host;

	 if((host = gethostbyname(name)) == NULL){
	 	dieWithError("gethostbyname() failed");
	 }

	 return *((unsigned long *)host->h_addr_list[0]);
}

int millis(struct timeval* t){
	return t->tv_sec*1000 + t->tv_usec/1000;
}

void dieWithError(char* message)
{
	fprintf(stderr, "%s\n", message);
	exit(1);
}