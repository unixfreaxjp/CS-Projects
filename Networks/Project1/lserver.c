#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXPENDING 5
#define RECV_SIZE 1024
#define PORT 14456

#define ID_LEN 9
#define FIRST_LEN 20
#define LAST_LEN 25
#define LOC_LEN 30

void dieWithError(char* errorMessage); //error handling function
void handleTCPClient(int clientSocket);  //TCP client handling function
char* doCommand(char* cmd);
void sendToClient(int sock, char* message);

//person linked list item
struct person {
	char id[ID_LEN+1];
	char first[FIRST_LEN + 1];
	char last[LAST_LEN + 1];
	char loc[LOC_LEN +1];
	struct person* next;
	struct person* prev;
};

//list head
struct person* head = NULL;
char loginName[10];
int clientCount = 0;
int loginCount = 0;
int totalPackets = 0;

//flags for close, quit , and list
unsigned int closeFlag = 0;
unsigned int quitFlag = 0;
unsigned int listFlag = 0;



int main(int argc, char** argv)
{

	FILE* outf;

	int serverSocket;	//Socket descriptor for server
	int clientSocket;	//Socket descriptor for client

	struct sockaddr_in serverAddr;	//local address
	struct sockaddr_in clientAddr;	//client address

	unsigned int clientLen;			//length of client address data structure

	//create socket for incoming connections
	if((serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		dieWithError("socket() failed");
	}

	//construct local address structure
	memset(&serverAddr, 0, sizeof(serverAddr));	//zero out structure
	serverAddr.sin_family = AF_INET;					//internet address family
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //any incoming interface
	serverAddr.sin_port = htons(PORT);	//local port

	//bind to the local address
	if(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0){
		dieWithError("bind() failed");
	}



	//set the size of the in-out parameter
	clientLen = sizeof(clientAddr);

	while(quitFlag == 0){
		//wait for client to connect
		printf("waiting for connection\n");

		//mark the socket so it will listen for incoming connections
		if(listen(serverSocket, MAXPENDING) < 0){
			dieWithError("listen() failed");
		}

		if((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen)) < 0){
			dieWithError("accept() failed");
		}

		//clientSocket is connected to a client
		printf("Handling client %s\n", inet_ntoa(clientAddr.sin_addr));
		handleTCPClient(clientSocket);
	}

	outf  = fopen("LClient.log", "a");
	struct person* pp = head;
	while(pp){
		fprintf(outf, "%s %s %s %s\n", pp->id, pp->first, pp->last, pp->loc);
		pp = pp->next;
	}
	fclose(outf);

	return 0;
}

void handleTCPClient(int clientSocket)
{
	char buffer[RECV_SIZE];
	int msgSize;
	int sent;

	int resLen = 0;
	//recieve message from client
	int count = 0;

	while(closeFlag == 0)
	{
				//see if there is more data to recieve
		if((msgSize = recv(clientSocket, buffer, RECV_SIZE, 0)) < 0){
			dieWithError("recv() failed");
		}
		buffer[msgSize] = '\0';

		if(msgSize == 0){
			break;
		}

		char* response = doCommand(buffer);
		if(response == NULL){
			response = "NOTHING";
		}
		resLen = strlen(response);

		if(send(clientSocket, response, resLen, 0) != resLen){
			dieWithError("send() sent a different number of bytes than expected");
		}
		loginCount++;
		totalPackets++;


		listFlag = 0;


	}
	closeFlag = 0;
	
	//recieve last packet to finish
	//recv(clientSocket, buffer, RECV_SIZE, 0);

	printf("closing\n");
	close(clientSocket);
	// }

}

void sendToClient(int sock, char* message){
	int resLen = strlen(message);
	if(send(sock, message, resLen, 0) != resLen){
		dieWithError("send() sent a different number of bytes than expected");
	}
}

char* doCommand(char* cmd)
{
	
	struct person* p = head;

	char* tokens[10];
	char* token;
	int tokenCount = 0;
	char* response = (char*)malloc(RECV_SIZE);
	token = strtok(cmd, " ");

	while(token){
		tokens[tokenCount] = token;
		tokenCount++;
		token = strtok(NULL, " ");
	}

	if(strcmp(tokens[0], "login") == 0){
		if(tokenCount >= 2){
			sprintf(response, "Hello %s!", tokens[1]);
			strcpy(loginName, tokens[1]);
			loginCount = 0;
			clientCount++;
			return response;
		}
	} else if (strcmp(tokens[0], "add") == 0) {
		struct person* newPerson = (struct person*)malloc(sizeof(struct person));
		char* id = tokens[1];
		char* first = tokens[2];
		char* last = tokens[3];
		char* loc = tokens[4];

		//check field lengths
		if(strlen(id) > ID_LEN){
			sprintf(response, "Person not added: id too long (%s)", id);
			return response;
		}
		if(strlen(first) > FIRST_LEN){
			sprintf(response, "Person not added: first name too long (%s)", first);
			return response;
		}
		if(strlen(last) > LAST_LEN){
			sprintf(response, "Person not added: last name too long (%s)", last);
			return response;
		}
		if(strlen(loc) > LOC_LEN){
			sprintf(response, "Person not added: location too long (%s)", loc);
			return response;
		}

		if(p != NULL){
			struct person* idcheck = p;
			do {
				if(strcmp(idcheck->id,id) == 0){
					sprintf(response, "Person not added: id already exists (%s)", id);
					return response;
				}
				idcheck = idcheck->next;
			} while(idcheck != NULL);
		}

		strcpy(newPerson->id, id);
		strcpy(newPerson->first, first);
		strcpy(newPerson->last, last);
		strcpy(newPerson->loc, loc);
		newPerson->next = NULL;
		newPerson->prev = NULL;

		if(head == NULL){
			head = newPerson;
		} else {
			unsigned int added = 0;

			while(p != NULL){
				if(strcmp(p->last, newPerson->last) > 0){
					added = 1;
					if(p->prev != NULL){
						p->prev->next = newPerson;
					}
					newPerson->prev = p->prev;
					p->prev = newPerson;
					newPerson->next = p;
					if(p == head){
						head = newPerson;
					}
					break;
				}
				p = p->next;
			}

			if(added == 0){
				p = head;
				while(p->next != NULL){
					p = p->next;
				}
				p->next = newPerson;
				newPerson->prev = p;
			} 
					
		}

		sprintf(response, "Added (%s, %s, %s, %s)", newPerson->id, newPerson->first, newPerson->last, newPerson->loc);

		return response;

	} else if (strcmp(tokens[0], "remove") == 0) {
		p = head;
		unsigned int removed = 0;
		while(p != NULL){
			if(strcmp(p->id, tokens[1]) == 0){
				removed = 1;
				if(p->prev != NULL){
					p->prev->next = p->next;
				}
				if(p->next != NULL){
					p->next->prev = p->prev;
				}
				if(p == head){
					head = p->next;
				}
				break;
			}
			p = p->next;
		}

		if(removed){
			sprintf(response, "Removed %s, %s", p->last, p->first);
			free(p);
		} else {
			sprintf(response, "Not removed: ID does not exist (%s)", tokens[1]);
		}
		return response;

	} else if (strcmp(tokens[0], "list") == 0) {
		sprintf(response,"LIST");
		//listFlag = 1;
		return response;

	} else if (strcmp(tokens[0], "quit") == 0) {
		printf("%s,%s\n", tokens[0],tokens[1]);
		if(tokenCount == 2 && strcmp(tokens[1], "EOF") == 0){
			sprintf(response, "QUITTING, %d clients processed, %d total packets sent.", clientCount, totalPackets);
			closeFlag = 1;
			quitFlag = 1;
			return response;
		} else {
			sprintf(response, "CLOSING Connection for %s. %d Packets sent.", loginName, loginCount);
			closeFlag = 1;
			loginCount = 0;
			return response;
		}

	} else {
		sprintf(response, "\'%s\'' is not a valid command", tokens[0]);
	}

	return response;

}

void dieWithError(char* errorMessage)
{
	fprintf(stderr, "%s\n", errorMessage);
	exit(1);
}