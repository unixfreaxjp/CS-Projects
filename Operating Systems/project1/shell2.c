#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <stdlib.h>
#include "runstat.h"

#define CMD_LEN 128
#define MAX_TOKENS 32

#define PROMPT "==>"
#define CMD_EXIT "exit"
#define CMD_CD "cd"
#define CMD_JOBS "jobs"
#define FLAG_BACKGROUND "&"

typedef struct bg_proc 
{
	int PID;
	char* cmd;
	struct bg_proc* next;
	struct bg_proc* prev;
} bg_proc_t ;

void remove_pid(bg_proc_t* first, bg_proc_t* last, int pid)
{
	bg_proc_t* p;
	p = first;

	while(p != NULL){
		if(p->PID == pid){
			if(p->prev != NULL) 
				p->prev->next = p->next;
			if(p->next != NULL)
				 p->next->prev = p->prev;
			if(p == first) 
				first = p->next;
			if(p == last)
				last = p->next;			
			break;			
		}
		p = p->next;						
	}
}

int main(int argc, char** argv)
{
	int exit = 0;
	char* input = NULL;

	size_t buflen = 0;
	ssize_t inputlen = 0;

	bg_proc_t* first = NULL;
	bg_proc_t* last = NULL;
	int bgcount = 0;

	while(!exit){
		printf(PROMPT);
		int background = 0;
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
				//printf("[%d] = %s\n", tokencount, token);
				tokencount++;
			}
			

			if(tokencount > 0 && strcmp(tokens[tokencount-1], FLAG_BACKGROUND) == 0){
				background = 1;
				tokens[tokencount-1] = '\0'; // remove the '&'			
			} else {	
				tokens[tokencount] = '\0';
			} 
			
			if(tokencount == 0)continue;

			if(strcmp(tokens[0], CMD_EXIT) == 0){
				exit = 1;
			} else if (strcmp(tokens[0], CMD_CD) == 0){
				int result = chdir(tokens[1]);
				if(result == -1){						
					printf("Invalid syntax for cd\n");
				}				
			} else if (strcmp(tokens[0], CMD_JOBS) == 0) { 
				bg_proc_t* p = first;
				int i = 0;
				while(p != NULL){
					i++;
					printf("[%d] %d %s\n", i, p->PID, p->cmd);
					p = p->next;
				}			
			} else {
				int childPID = fork();
				if(childPID < 0){
					printf("fork() failed");
				} else if(childPID == 0){
					runstat(tokens);
					return 0;//exit(0); // this branch is a child process, so exit
				} else {
					int result = 0;
					if(!background){
						waitpid(childPID, NULL, 0);
					} else {
						bgcount++;
						bg_proc_t* new_proc = (bg_proc_t*)malloc(sizeof(bg_proc_t));
						new_proc->PID = childPID;
						new_proc->cmd = tokens[0];
						if(last == NULL){
							last = new_proc;
							first = last;
						} else {
							last->next = new_proc;
							new_proc->prev = last;
							last = new_proc;
						}		
					}	
					
					int pid = -1;
					while((pid = wait3(NULL, WNOHANG, NULL)) > 0) {
						remove_pid(first, last, pid);
						bgcount--;
					}				
				}
			}
					
		}

		input = NULL;
	}
	
	if(bgcount > 0){
		printf("Waiting for %d incomplete task(s) before exiting.\n", bgcount);
		fflush(stdout);
		while(bgcount > 0){
			int pid; 
			pid = wait3(NULL, WNOHANG, NULL);
			if(pid > 0) {
				remove_pid(first, last, pid);
				bgcount--;
			}
		}
	}
	
	return 0;
}

