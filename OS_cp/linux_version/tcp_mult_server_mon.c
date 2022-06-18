// C program for the Server Side

// inet_addr
#include <arpa/inet.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

//Shared resource
volatile int val = 0;

#define SLEEP_TIME 3
#define WRITING_THREADS 1000
#define READING_THREADS 1000
#define TRY_TO_ACCESS 1

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
	//if (wcnt == 1 || waitw > 0) {
	if (wcnt == 1) {
		// incrementing waiting readers
		waitr++;

		// reader suspended
		pthread_cond_wait(&canread, &condlock);
		waitr--;
	}

	// else reader reads the resource
	rcnt++;
	printf("Reader %d is reading | readers in total: %d | writers in total: %d | readers in queue: %d | writers in queue: %d\n", i, rcnt, wcnt, waitr, waitw);
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
	printf("Writer %d is writing | readers in total: %d | writers in total: %d | readers in queue: %d | writers in queue: %d\n", i, rcnt, wcnt, waitr, waitw);
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

// Reader Function
void* reader(void* param)
{
	int c = 0;
	int i = *(int*)param;

	while (c < TRY_TO_ACCESS) {
		sleep(SLEEP_TIME%(SLEEP_TIME+1));
		beginread(i);
		printf(">>> Reader %d: val value: %d\n", i, val);
		endread(i);
		c++;
	}
}

// Writer Function
void* writer(void* param)
{
	int c = 0;
	int i = *(int*)param;

	// each writer attempts to write several times
	while (c < TRY_TO_ACCESS) {
		sleep(SLEEP_TIME%(SLEEP_TIME+1));
		beginwrite(i);
		printf(">>> Writer %d: val incremented from %d to %d\n", i, val, val+1);
		val++;
		endwrite(i);
		c++;
	}
}

// Driver Code
int main()
{
	// Initialize variables
	int connected_readers = 0;
	int connected_writers = 0;
	int serverSocket, newSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;

	pthread_cond_init(&canread, NULL);
	pthread_cond_init(&canwrite, NULL);
	pthread_mutex_init(&condlock, NULL);

	pthread_t writerthreads[WRITING_THREADS];
	pthread_t readerthreads[READING_THREADS];

	socklen_t addr_size;

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8989);

	// Bind the socket to the
	// address and port number.
	bind(serverSocket,
		(struct sockaddr*)&serverAddr,
		sizeof(serverAddr));

	// Listen on the socket,
	// with 40 max connection
	// requests queued
	if (listen(serverSocket, 500) == 0)
		printf("Listening\n");
	else
		printf("Error\n");

	while (1) {
		
		addr_size = sizeof(serverStorage);

		// Extract the first
		// connection in the queue
		newSocket = accept(serverSocket,
						(struct sockaddr*)&serverStorage,
						&addr_size);

		int choice = 0;
		recv(newSocket, &choice, sizeof(choice), 0);

		if (choice == 1) {
			// Creater readers thread
			if (pthread_create(&readerthreads[connected_readers], NULL,
							reader, &connected_readers)
				!= 0)

				// Error in creating thread
				printf("Failed to create thread\n");
			pthread_detach(readerthreads[connected_readers]);
			connected_readers++;
		}
		else if (choice == 2) {
			// Create writers thread
			if (pthread_create(&writerthreads[connected_writers], NULL,
							writer, &connected_writers)
				!= 0)

				// Error in creating thread
				printf("Failed to create thread\n");
			pthread_detach(writerthreads[connected_writers]);
			connected_writers++;
		}
	}

	close(serverSocket);

	pthread_cond_destroy(&canread);
	pthread_cond_destroy(&canwrite);
	pthread_mutex_destroy(&condlock);

	return 0;
}
