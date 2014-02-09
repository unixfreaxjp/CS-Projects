#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "runstat.h"

#define CMD_LEN 128
#define MAX_TOKENS 32

#define PROMPT "==>"
#define CMD_EXIT "exit"
#define CMD_CD "cd"

int main(int argc, char** argv)
{
	int exit = 0;
	char* input = NULL;

	size_t buflen = 0;
	ssize_t inputlen = 0;


	while(!exit){
		printf(PROMPT);
		
		inputlen = getline(&input, &buflen, stdin);
		//printf("READ:%s", input);
		
		if(inputlen == -1){
			break;		
		} else if (inputlen > 128){
			printf("Command was too long (> 128 characters)");	
		} else {

			char* tokens[MAX_TOKENS];
			char* token = NULL;
			int tokencount = 0;
	
	
			printf("INPUT:%s\n", input);
			input = strtok(input, "\n"); //remove endline character using strtok

			while((token = strtok(input, " ")) != NULL) {
				input = NULL;
				if(tokencount > MAX_TOKENS){
					printf("Too many tokens in command (> 32 tokens)\n");
					break;				
				}				
				tokens[tokencount] = token;
				tokencount++;
			}
			tokens[tokencount] = '\0';
			/*int i;
			for(i = 0; i < tokencount; i++){
				printf("token %d = %s\n", i, tokens[i]);			
			}*/

			if(strcmp(tokens[0], CMD_EXIT) == 0){
				exit = 1;
			} else if (strcmp(tokens[0], CMD_CD) == 0){
				int result = chdir(tokens[1]);
				if(result == -1){						
					printf("Invalid syntax for cd\n");
				}				
			} else {
				int childPID = fork();
				if(childPID < 0){
					printf("fork() failed");
				} else if(childPID == 0){
					runstat(tokens);
					exit = 1; // this branch is a child process, so exit
				} else {
					wait(childPID);				
				}
			}
					
		}

		input = NULL;
	
	}
	return 0;
}

