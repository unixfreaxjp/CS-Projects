#include "bathroom.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <limits.h>

#define T_MS(t) (t.tv_sec*1000 + t.tv_usec/1000)
#define T_US(t) (t.tv_sec*1000000 + t.tv_usec)

typedef struct {
	int num;
	gender p_gender;
	int p_count;
} person;

void *Individual(void* args);
double box_muller(double u, double s);

int nUsers, meanLoopCount, meanArrival, meanStay;

pthread_mutex_t join_mutex;
int finishCount;


int main(int argc, char** argv)
{

	srand(time(NULL)); // seed random - was geting same numbers every run before!
	srand48(time(NULL));
	
	pthread_mutex_init(&join_mutex, 0);
	finishCount = 0;
	
	
	if(argc < 5){
		printf("Format Error : bathroomSim [nUsers] [meanLoopCount] [meanArrival] [meanStay]\n");
		return 1;
	}
		
	nUsers = atoi(argv[1]);
	meanLoopCount = atoi(argv[2]);
	meanArrival = atoi(argv[3]);
	meanStay = atoi(argv[4]);
	
		
	printf("args: %d, %d, %d, %d\n", nUsers, meanLoopCount, meanArrival, meanStay);
	
	int threads = 0;
	pthread_t tids[nUsers];
	Initialize();
	
	for(threads = 0; threads < nUsers; threads++) {
		person *p = malloc(sizeof(person));
		p->num = threads;
		p->p_gender = (rand() < RAND_MAX/2)?male:female;
		p->p_count = (int)box_muller((double)meanLoopCount, (double)meanLoopCount/2);
		if(p->p_count == 0)p->p_count = 1;
		pthread_create(&tids[threads], NULL, Individual, (void*)p);
	}
	
	
	while(finishCount < nUsers); // just loop while the other threads are running
		
	for(threads = 0; threads < nUsers; threads++) {
		pthread_join(tids[threads], 0);
	}
	
	/*while(finishCount < nUsers){		
	pthread_mutex_lock(&join_mutex);
		pthread_cond_wait(&join_cond, &join_mutex);
	
		pthread_join(finished, 0);
		finished = 0;
		finishCount++;
		printf("\t\t\t\tTC = %d    F = %d\n", threads, finishCount);
			
	pthread_mutex_unlock(&join_mutex);
	}*/
	
	
	Finalize();
	
	pthread_mutex_destroy(&join_mutex);
	
	
	return 0;	
}


void *Individual(void* args)
{
	person* me = (person*)(args);
	//printf("start thread %ld (%s)x%d\n", pthread_self(), gender_str(me->p_gender), me->p_count);
	int i;
	int arrival, stay;
	
	struct timeval start, end;
	
	int wait, minWait, avgWait, maxWait;
	minWait = INT_MAX;
	avgWait = 0;
	maxWait = INT_MIN;
	
	for(i = 0; i < me->p_count; i++){		
		arrival = (int) box_muller(meanArrival, meanArrival/2);
		stay = (int) box_muller(meanStay, meanStay/2);
		
		usleep(arrival);	
		
		gettimeofday(&start, NULL);	
		Enter(me->p_gender);
		gettimeofday(&end, NULL);
		
		wait = T_US(end)-T_US(start);
		minWait = (wait < minWait)?wait:minWait;
		maxWait = (wait > maxWait)?wait:maxWait;
		avgWait += wait/(me->p_count);
		
		usleep(stay);		
		Leave();
	}
	
	pthread_mutex_lock(&join_mutex);
	
	printf("Thread %d (%s): \n\tloops = %d \n\tmin = %dus \n\tmax = %dus \n\tavg = %dus \n\n", me->num, gender_str(me->p_gender), me->p_count, minWait, maxWait, avgWait);
	finishCount++;
	pthread_mutex_unlock(&join_mutex);
	
	return NULL;
	
}

double box_muller(double u, double s)
{
	double a, b, z;
	a = drand48();
	b = drand48();
	z = s * (sqrt(-2 * log(a)) * cos(2 * 3.14159 * b)) + u;
	if(z < 0) z = 0;
	return z;
}
