// C program for the Client Side
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

// inet_addr
#include <arpa/inet.h>
#include <unistd.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>

#define SLEEP_TIME 0
#define THREADS_NUM 500

// Function to send data to
// server socket.
void* clienthread(void* args)
{

	int client_request = *((int*)args);
	int network_socket;

	// Create a stream socket
	network_socket = socket(AF_INET,
							SOCK_STREAM, 0);

	// Initialise port number and address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(8989);

	// Initiate a socket connection
	int connection_status = connect(network_socket,
									(struct sockaddr*)&server_address,
									sizeof(server_address));

	// Check for connection error
	if (connection_status < 0) {
		puts("Error\n");
		return 0;
	}

	printf("\nConnection established\n");

	// Send data to the socket
	send(network_socket, &client_request,
		sizeof(client_request), 0);

	// Close the connection
	close(network_socket);
	//pthread_exit(NULL);

	return 0;
}

// Driver Code
int main()
{
	
	pthread_t threads[THREADS_NUM];

	// Create connection
	// depending on the input
	for (int i=0; i<THREADS_NUM; i++) {
        printf("WRITER %d is on track\n", i);
		int client_request = 2;

		// Create thread
		pthread_create(&threads[i], NULL,
					clienthread,
					&client_request);
		pthread_join(threads[i], NULL);
		sleep(SLEEP_TIME);
	}

	// Suspend execution of
	// calling thread
	//for (int i=0; i < THREADS_NUM; i++)
    //    pthread_join(threads[i], NULL);
}
