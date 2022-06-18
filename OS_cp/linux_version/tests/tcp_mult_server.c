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

typedef struct use_mutex_tag {
    pthread_mutex_t mutex;
	int tid;
} use_mutex_t;

// Semaphore variables
pthread_t writerthreads[100];
pthread_t readerthreads[100];

// Reader Function
void* reader(void* param)
{
	use_mutex_t *arg = (use_mutex_t*) param;
    pthread_mutex_lock(&(arg->mutex));

	printf("TID: %d >>> READER: val value is %d\n", arg->tid, val);
	pthread_mutex_unlock(&(arg->mutex));
	pthread_exit(NULL);
}

// Writer Function
void* writer(void* param)
{
	use_mutex_t *arg = (use_mutex_t*) param;
    pthread_mutex_lock(&(arg->mutex));
	printf("TID: %d >>> WRITER: increment val from %d to %d", arg->tid, val++, val);
	pthread_mutex_unlock(&(arg->mutex));
	pthread_exit(NULL);
}

// Driver Code
int main()
{
	// Initialize variables
	int serverSocket, newSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;

	use_mutex_t param;
	pthread_mutex_init(&(param.mutex), NULL);

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
	if (listen(serverSocket, 50) == 0)
		printf("Listening\n");
	else
		printf("Error\n");

	int i = 0;

	while (1) {
		addr_size = sizeof(serverStorage);

		// Extract the first
		// connection in the queue
		newSocket = accept(serverSocket,
						(struct sockaddr*)&serverStorage,
						&addr_size);
		//if (newSocket != -1)
		//	printf("Connection established!\n");

		int choice = 0;
		recv(newSocket, &choice, sizeof(choice), 0);

		if (choice == 1) {
			param.tid = i;
			// Creater readers thread
			if (pthread_create(&readerthreads[i], NULL,
							reader, &param)
				!= 0)

				// Error in creating thread
				printf("Failed to create thread\n");
			i++;
		}
		else if (choice == 2) {
			param.tid = i;
			// Create writers thread
			if (pthread_create(&writerthreads[i], NULL,
							writer, &param)
				!= 0)

				// Error in creating thread
				printf("Failed to create thread\n");
			i++;
		}

		if (i >= 50) {
			// Update i
			i = 0;

			while (i < 50) {
				// Suspend execution of
				// the calling thread
				// until the target
				// thread terminates
				pthread_join(writerthreads[i++],
							NULL);
				pthread_join(readerthreads[i++],
							NULL);
			}

			// Update i
			i = 0;
		}
	}

	return 0;
}
