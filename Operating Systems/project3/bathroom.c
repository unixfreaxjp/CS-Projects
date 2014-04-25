#include "bathroom.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#define T_US(t) (t.tv_sec*1000000 + t.tv_usec)

int count;
gender current_gender;

pthread_mutex_t count_mutex;
pthread_cond_t empty;

int usages;
long vacant_time;
long occupied_time;
long total_time;

long avg_count;

int queue_size;
long avg_queue;
long queue_total_time;

struct timeval vacant_start;
struct timeval vacant_end;

struct timeval queue_t;
struct timeval occupancy_t;
struct timeval now;

void track_queue_size()
{
	gettimeofday(&now, NULL);
	long elapsed = (T_US(now)-T_US(queue_t));
	avg_queue += queue_size * elapsed;
	queue_total_time += elapsed;	
	queue_t = now;
}

void track_occupancy()
{
	gettimeofday(&now, NULL);
	long elapsed = (T_US(now)-T_US(occupancy_t));
	avg_count += count * elapsed;
	occupancy_t = now;
}


void Initialize(){
	count = 0;
	current_gender = male;
	
	usages = 0;
	vacant_time  = 0;
	occupied_time = 0;
	total_time = 0;
	queue_size = 0;
	avg_queue = 0;
	avg_count = 0;
	gettimeofday(&vacant_start, NULL);
	queue_t = vacant_start;
	occupancy_t = vacant_start;
	pthread_mutex_init(&count_mutex, 0);
	pthread_cond_init(&empty, 0);
}

void Enter(gender g){
	pthread_mutex_lock(&count_mutex);
	if(current_gender != g && count > 0){
		track_queue_size();
		queue_size++;
				
		pthread_cond_wait(&empty, &count_mutex);
		track_queue_size();
		queue_size--;
	}
	if(count == 0){
		gettimeofday(&vacant_end, NULL);
		vacant_time += T_US(vacant_end)-T_US(vacant_start);
		current_gender = g;
	}
	track_occupancy();
	count++;
	usages++;
	//printf("t%d (%s) Entered count=%d\n", (unsigned int)pthread_self(), gender_str(g), count);
	pthread_mutex_unlock(&count_mutex);
		
}

void Leave(){
	pthread_mutex_lock(&count_mutex);
	track_occupancy();
	count--;
	if(count == 0){
		gettimeofday(&vacant_start, NULL);
		occupied_time += T_US(vacant_start)-T_US(vacant_end);
		pthread_cond_broadcast(&empty);
	}	
	//printf("t%d (%s) Left    count=%d\n", (unsigned int)pthread_self(), gender_str(current_gender), count);
	pthread_mutex_unlock(&count_mutex);
	
}

/*
         +the number of usages of the bathroom
         + the total amount of time that the bathroom was vacant
         + the total amount of time that the bathroom was occupied
         + the average queue length
         + and the average number of persons in the bathroom at the same time.

*/
void Finalize(){

	printf("Usages: %d\n", usages);
	printf("Vacant time: %ldus\n", vacant_time);
	printf("Occupied time: %ldus\n", occupied_time);
	printf("Average queue: %ld\n", (avg_queue/queue_total_time));
	printf("Average occupancy: %ld\n", (avg_count/(vacant_time+occupied_time)));
	
	pthread_mutex_destroy(&count_mutex);
	pthread_cond_destroy(&empty);
	
}




