// Reader-Writer problem using monitors
// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SLEEP_TIME 1
#define WRITING_THREADS 100
#define READING_THREADS 25

//shared resource
volatile int val = 0;


// no. of readers
int rcnt = 0;
// no. of writers
int wcnt = 0;
// no. of readers waiting
int waitr = 0;
// no. of writers waiting
int waitw = 0;

// condition variable to check whether reader can read
pthread_cond_t canread;
// condition variable to check whether writer can write
pthread_cond_t canwrite;
// mutex for synchronization
pthread_mutex_t condlock;



// mutex provide synchronization so that no other thread
// can change the value of data
void beginread(int i)
{
	pthread_mutex_lock(&condlock);

	// if there are active or waiting writers
	if (wcnt == 1 || waitw > 0) {
		// incrementing waiting readers
		waitr++;

		// reader suspended
		pthread_cond_wait(&canread, &condlock);
		waitr--;
	}

	// else reader reads the resource
	rcnt++;
	printf("Reader %d is reading | readers in total: %d | writers in total: %d | readers in queue: %d | writer in queue: %d\n", i, rcnt, wcnt, waitr, waitw);
	pthread_mutex_unlock(&condlock);
	pthread_cond_broadcast(&canread);
}

void endread(int i)
{

	// if there are no readers left then writer enters monitor
	pthread_mutex_lock(&condlock);

	if (--rcnt == 0)
		pthread_cond_signal(&canwrite);

	pthread_mutex_unlock(&condlock);
}
	
void beginwrite(int i)
{
	pthread_mutex_lock(&condlock);

	// a writer can enter when there are no active
	// or waiting readers or other writer
	if (wcnt == 1 || rcnt > 0) {
		++waitw;
		pthread_cond_wait(&canwrite, &condlock);
		--waitw;
	}
	wcnt = 1;
	printf("Writer %d is writing | readers in total: %d | writers in total: %d | readers in queue: %d | writer in queue: %d\n", i, rcnt, wcnt, waitr, waitw);
	pthread_mutex_unlock(&condlock);
}

void endwrite(int i)
{
	pthread_mutex_lock(&condlock);
	wcnt = 0;

	// if any readers are waiting, threads are unblocked
	if (waitr > 0)
		pthread_cond_signal(&canread);
	else
		pthread_cond_signal(&canwrite);
	pthread_mutex_unlock(&condlock);
}

void* reader(void* id)
{
	int c = 0;
	int i = *(int*)id;

	// each reader attempts to read 5 times
	while (c < 5) {
		sleep(SLEEP_TIME);
		beginread(i);
		printf(">>> Reader %d: val value: %d\n", i, val);
		endread(i);
		c++;
	}
	/*
	sleep(SLEEP_TIME);
	beginread(i);
	//printf(">>> Reader %d\n", i);
	printf(">>> Reader %d: val value: %d\n", i, val);
	endread(i);
	*/
}

void* writer(void* id)
{
	int c = 0;
	int i = *(int*)id;

	// each writer attempts to write 5 times
	while (c < 5) {
		sleep(SLEEP_TIME);
		beginwrite(i);
		printf(">>> Writer %d: val incremented from %d to %d\n", i, val, val+1);
		val++;
		endwrite(i);
		c++;
	}
	/*
	sleep(SLEEP_TIME);
	beginwrite(i);
	//printf(">>> Writer %d\n", i);
	printf(">>> Writer %d: val incremented from %d to %d\n", i, val, val+1);
	val++;
	endwrite(i);
	*/
}

int main()
{
	pthread_cond_init(&canread, NULL);
	pthread_cond_init(&canwrite, NULL);
	pthread_mutex_init(&condlock, NULL);

	pthread_t r[READING_THREADS], w[WRITING_THREADS];
	int id_r[READING_THREADS], id_w[WRITING_THREADS];

	for (int i = 0; i < READING_THREADS; i++) {
		id_r[i] = i;

		// creating threads which execute reader function
		pthread_create(&r[i], NULL, &reader, &id_r[i]);
	}
	for (int i = 0; i < WRITING_THREADS; i++) {
		id_w[i] = i;

		// creating threads which execute writer function
		pthread_create(&w[i], NULL, &writer, &id_w[i]);
	}

	for (int i = 0; i < READING_THREADS; i++) {
		pthread_join(r[i], NULL);
	}
	for (int i = 0; i < WRITING_THREADS; i++) {
		pthread_join(w[i], NULL);
	}
	pthread_cond_destroy(&canread);
	pthread_cond_destroy(&canwrite);
	pthread_mutex_destroy(&condlock);
}
