#include <stdio.h>
#include <unistd.h>
#include "prinfo.h"

void printprinfo(int tabs);

int main(int argc, char** argv)
{
	int i, j;
	int tabs = 0;
	for( i = 0; i < 3; i++){
		int pid = fork();
		if(pid == 0){
			tabs++;
			if(i == 0){
				for(j = 0; j < 2; j++){
					int npid = fork();
					if(npid == 0){
						tabs++;
						getchar();
						printprinfo(tabs);
					}
				}
			}
			getchar();
			printprinfo(tabs);
		}		
	}
	getchar();
	printprinfo(tabs);
	

	
	return 0;	
}

void printprinfo(int tabs)
{
	char tab[tabs+1];
	int i;
	for(i = 0; i < tabs; i++)tab[i] = *"\t";
	tab[tabs] = *"\0";
	
	struct prinfo info;
	long result = getprinfo(&info);
	printf("\n");
	printf("%sstate = %ld \n", tab, info.state);
	printf("%spid = %d \n", tab, info.pid);
	printf("%sparent pid = %d \n", tab,info.parent_pid);
	printf("%syoungest child = %d \n",tab, info.youngest_child);
	printf("%syounger sibling = %d \n",tab, info.younger_sibling);
	printf("%solder sibling = %d \n",tab, info.older_sibling);
	printf("%suid = %d \n", tab,info.uid);
	printf("%sstart time = %lld \n", tab,info.start_time);
	printf("%suser time = %lld \n",tab, info.user_time);
	printf("%ssys time = %lld \n", tab,info.sys_time);
	printf("%schildren utime = %lld \n",tab, info.cutime);
	printf("%schildren stime = %lld \n",tab, info.cstime);
	printf("\n");
	
}
