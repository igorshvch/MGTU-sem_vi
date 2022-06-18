#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define SAW 0
#define SAR 1
#define SWW 2
#define SWR 3

#define rn 6
#define wn 3
#define N 3

struct sembuf start_read[5] = {
    {SWR, 1, SEM_UNDO},
    {SAW, 0, SEM_UNDO},
    {SWW, 0, SEM_UNDO},
    {SWR, -1, SEM_UNDO},
    {SAR, 1, SEM_UNDO}
};

struct sembuf stop_read[1] = {
    {SAR, -1, SEM_UNDO}
};

struct sembuf start_write[5] = {
    {SWW, 1, SEM_UNDO},
    {SAR, 0, SEM_UNDO},
    {SAW, 0, SEM_UNDO},
    {SWW, -1, SEM_UNDO},
    {SAW, 1, SEM_UNDO}
};

struct sembuf stop_write[1] = {
    {SAW, -1, SEM_UNDO}
};

void reader(int fdm, int fds, int cur_r) {
    int* addr = (int*)shmat(fdm, 0, 0);
    
    if ((void*)addr == (void *) -1) {
        perror("shmat error");
        exit(1);
    }
    
    for (int i = 0; i < N; i++) {
        sleep(rand() % 3);
        
        if (semop(fds, start_read, 5) == -1) {
            perror("start read error");
            exit(1);
        }
        
        printf("reader %d: %d\n", cur_r, *addr);
        if (semop(fds, stop_read, 1) == -1) {
            perror("stop read error");
            exit(1);
        }
    }
    
    if(shmdt(addr)) {
        perror("shmdt error");
        exit(1);
    }
    printf("end of reader %d\n", cur_r);
}

void writer(int fdm, int fds, int cur_w) {
    int* addr = (int*)shmat(fdm, 0, 0);
    
    if ((void*)addr == (void *) -1) {
        perror("shmat error");
        exit(1);
    }
    
    for (int i = 0; i < N; i++) {
        sleep(rand() % 3);
        
        if (semop(fds, start_write, 5) == -1) {
            perror("start write error");
            exit(1);
        }
        
        (*addr)++;
        printf("writer %d: %d\n", cur_w, *addr);
        
        if (semop(fds, stop_write, 1) == -1) {
            perror("stop write error");
            exit(1);
        }
    }
    
    if(shmdt(addr)) {
        perror("shmdt error");
        exit(1);
    }
    printf("end of writer %d\n", cur_w);
}

int main(void) {
    int perms = S_IRWXU|S_IRWXG|S_IRWXO;

    int fds = semget(IPC_PRIVATE, 4, IPC_CREAT|perms);
    if (fds == -1) {
        perror("semget error");
        exit(1);
    }

    int ctl_saw = semctl(fds, SAW, SETVAL, 0);
    int ctl_sar = semctl(fds, SAR, SETVAL, 0);
    int ctl_sww = semctl(fds, SWW, SETVAL, 0);
    int ctl_swr = semctl(fds, SWR, SETVAL, 0);
    
    if (ctl_saw == -1 || ctl_sar == -1 || ctl_sww == -1 || ctl_swr == -1) {
        perror("semctl error");
        exit(1);
    }

    int fdm = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|perms);
    if (fdm == -1) {
        perror("shmget error");
        exit(1);
    }
    
    int* addr = (int*)shmat(fdm, 0, 0);
    if ((void*)addr == (void *) -1) {
        perror("shmat error");
        exit(1);
    }
    *addr = 0;
    
    for (int i = 0; i < wn; i++) {
        int pid = fork();
        if (pid == -1) {
            perror("fork error");
            exit(1);
        } else if (pid == 0) {
            writer(fdm, fds, i);
            return 0;
        } else {
            printf("%d forked writer %d\n", getpid(), pid);
        }
    }
    
    for (int i = 0; i < rn; i++) {
        int pid = fork();
        if (pid == -1) {
            perror("fork error");
            exit(1);
        } else if (pid == 0) {
            reader(fdm, fds, i);
            return 0;
        } else {
            printf("%d forked reader %d\n", getpid(), pid);
        }
    }

    for (int i = 0; i < rn + wn; i++) {
        int status;
        int child_id = wait(&status);
        printf("finishing child: PID %d\n", child_id);
        if (WIFEXITED(status))
          printf("Normal exit with exit code %d\n", WEXITSTATUS(status));
        else
          printf("Not normal exit\n");
        if (WIFSIGNALED(status))
          printf("Signaled exit with signal number %d\n", WTERMSIG(status));
        else
          printf("Not signaled exit\n");
        if (WIFSTOPPED(status))
          printf("Process stepped with signal number %d\n", WSTOPSIG(status));
        else
          printf("Process not stopped\n");
    }
    
    if(shmdt(addr)) {
        perror("shmdt error");
        exit(1);
    }
    
    return 0;
}
