#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
// bitmap implementation and declared a BIT_SIZE 32 
#define BIT_SIZE 32
long getBitmapSize(long size) {
        return ( size/BIT_SIZE + (!!(size%BIT_SIZE)) );
}

void set_bit(uint32_t *bitmap, int index) {
    bitmap[index / BIT_SIZE] |= 1 << (index % BIT_SIZE);
}

void clear_bit(uint32_t *bitmap, int index) {
    bitmap[index / BIT_SIZE] &= ~(1 << (index % BIT_SIZE));
}

int get_bit(uint32_t *bitmap, int index) {
    return 1 & (bitmap[index / BIT_SIZE] >> (index % BIT_SIZE));
}

// pid manager functions
#define MIN 100
#define MAX 1000
int pid_map;
pthread_mutex_t lock;
// Creates and initializes a data structure for representing pids 
int allocate_map(long size) {
	pid_map = (int *)calloc(getBitmapSize(size), sizeof(int));
	if(pid_map == NULL)
		return -1;//returns —1 if unsuccessfull
	return 1;//1 if successfullly created pid
}

// Allocates and returns a pid
//returns —1 if unable to allocate a pid only in case if all pids are in use
int allocate_pid() {
	for(int i=MIN; i<MAX; i++) {
		if(get_bit(pid_map, i-1) == 0) {
			set_bit(pid_map, i-1);
			return i;
		}
	}
	return -1; // pid_map if full return -1
}

// function to Release a pid
void release_pid(int pid) {
	// release pid
	clear_bit(pid_map, pid-1);
}

// function to implement process
void* processFunc(void *arg) {
	int tid = (int *)arg;
	sleep(2);
	pthread_mutex_lock(&lock);
		int pid = allocate_pid();
		if(pid == -1) {
			printf("Thread %d : Unable to allocate pid! All PIDs in use.\n", tid);
			pthread_mutex_unlock(&lock);
			pthread_exit(NULL);
		}
	pthread_mutex_unlock(&lock);

	printf("Thread %d : Allocated PID %d \n", tid, pid);
	printf("Thread %d : Running code \n", tid);
	sleep(1);
	printf("Thread %d : Code Completed \n", tid);

	pthread_mutex_lock(&lock);
		printf("Thread %d : Releasing allocated PID %d\n", tid, pid);
		release_pid(pid);	
		printf("Thread %d : PID %d Released\n", tid, pid);
	pthread_mutex_unlock(&lock);
	pthread_exit(NULL);
}

int main(int argc, char** arv) {
	allocate_map(MAX);
	int noThreads;
	printf("How many threads you want to create? \n");
	scanf("%d", &noThreads);
	pthread_t threads[noThreads];
	for(int i=1; i<=noThreads; i++)
		pthread_create(&threads[i], NULL, processFunc, (void *)i);

	for(int i=1; i<=noThreads; i++)
		pthread_join(threads[i], NULL);

	printf("All threads executed\n");

	return 0;
}
