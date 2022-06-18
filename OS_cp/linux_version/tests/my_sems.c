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

#define SLEEP_TIME 1
#define ITERATIONS 5

sem_t semaphore;
 
static int counter = 0;
 
void* worker1(void* args) {
    int i;
    int local;
    for (i = 0; i < ITERATIONS; i++) {
        sem_wait(&semaphore);
        local = counter;
        printf("worker1 - %d\n", local);
        local++;
        counter = local;
        sleep(SLEEP_TIME);
        sem_post(&semaphore);
    }
}
 
void* worker2(void* args) {
    int i;
    int local;
    for (i = 0; i < ITERATIONS; i++) {
        sem_wait(&semaphore);
        local = counter;
        printf("worker 2 - %d\n", local);
        local--;
        counter = local;
        sleep(SLEEP_TIME);
        sem_post(&semaphore);
    }
}
 
void main() {
    pthread_t thread1;
    pthread_t thread2;
     
    sem_init(&semaphore, 0, 1);
 
    pthread_create(&thread1, NULL, worker1, NULL);
    pthread_create(&thread2, NULL, worker2, NULL);
 
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
 
    sem_destroy(&semaphore);
    printf("== %d", counter);
    getchar();
}