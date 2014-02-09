#include "runstat.h"

long timeval_to_ms(struct timeval* t)
{
	return (t->tv_sec*1000 + t->tv_usec/1000);
}

void runstat(char** argv)
{
	//printf("------RUNSTAT------\n");
	int childPID;
	struct timeval before, after;
	struct rusage resource_usage;

	childPID = fork();
	
	gettimeofday(&before, NULL);
	
	if(childPID < 0){
		printf("fork() failed\n");
	} else if(childPID == 0){
		//printf("exec %s\n", argv[0] );

		//int i = -1;
		//while(argv[++i] != 0)printf("%s\n", argv[i]);

		execvp(argv[0], argv);
	} else {
		//printf("wait(%d)", childPID);
		wait(childPID);
		gettimeofday(&after, NULL);

		getrusage(RUSAGE_CHILDREN , &resource_usage);

		long elapsed = timeval_to_ms(&after)-timeval_to_ms(&before);
		long userTime = timeval_to_ms(&(resource_usage.ru_utime));
		long cpuTime = timeval_to_ms(&(resource_usage.ru_stime));		
		
		printf("\nStats for child %d (%s) :\n", childPID, argv[0]);
		printf("\tElapsed time: %ldms\n", elapsed);
		printf("\tUser CPU time: %ldms\n", userTime);
		printf("\tSystem CPU time: %ldms\n", cpuTime);
		printf("\tInvoluntary context switches: %ld\n", resource_usage.ru_nivcsw);
		printf("\tVoluntary context switches: %ld\n", resource_usage.ru_nvcsw);
		printf("\tMajor page faults: %ld\n", resource_usage.ru_majflt);
		printf("\tMinor page faults: %ld\n", resource_usage.ru_minflt);	
		
	}
}

