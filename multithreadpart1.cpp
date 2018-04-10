#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#define MIN 100
#define MAX 1000

uint32_t *pid_map;
pthread_mutex_t lock;

// Creates and initializes a data structure for representing pids; returns —1 if unsuccessfull, 1 if successfull 
int allocate_map(long long size) {
	pid_map = (uint32_t *)calloc(getBitmapSize(size), sizeof(uint32_t));
	if(pid_map == NULL)
		return -1;
	return 1;
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
int main(int argc, char* arv) {
	allocate_map(MAX);
	int noThreads;
	printf("How many threads you want to create? \n");
	scanf("%d", &noThreads);
	pthread_t threads[noThreads];
	for(int i=1; i<=noThreads; i++)
		pthread_create(&threads[i], NULL,processFunc, (void *)i);
	for(int i=1; i<=noThreads; i++)
		pthread_join(threads[i], NULL);
	printf("All threads executed\n");
	return 0;
}
