#include <stdio.h>
#include <stdbool.h>
#include <windows.h>

#define rn 20
#define wn 2
#define rd_cnt 1
#define wrt_cnt 1

HANDLE writers_sore[wn];
HANDLE readers_store[rn];

HANDLE mutex;
HANDLE can_read_event; // событие, пропускающее читателей
HANDLE can_write_event; // событие, пропускающее писателей

volatile int val = 0;

volatile LONG queue_canread = 0; //счетчик очереди читателей
volatile LONG queue_canwrite = 0; //счетчик очереди писателей
volatile LONG readers = 0; // переменная-счётчик количества читателей, защищаемая мьютексом
bool writerlock = false;

//открытие работы читателя
void start_read() {
    InterlockedIncrement(&queue_canread);
    if (writerlock || WaitForSingleObject(can_write_event, 0) == WAIT_OBJECT_0) {
        WaitForSingleObject(can_read_event, INFINITE);
    }
    
    WaitForSingleObject(mutex, INFINITE);
    
    InterlockedDecrement(&queue_canread);
    InterlockedIncrement(&readers);
    SetEvent(can_read_event);
    
    ReleaseMutex(mutex);
}

//закрытие работы читателя
void stop_read() {
    InterlockedDecrement(&readers);
    if (queue_canread== 0) {
        SetEvent(can_write_event);
    }
}

//чтение
DWORD WINAPI reader(LPVOID lpParams) {
    for (int i = 0; i < rd_cnt; i++) {
        Sleep(100);
        start_read();
        printf("reader %ld: %ld\tWRITERS in queue: %ld\tREADERS in queue: %ld\tREADERS: %ld\n", (int) lpParams, val, queue_canwrite, queue_canread, readers);
        stop_read();
    }
    
    return 0;
}

//откртие работы писателя
void start_write() {
    InterlockedIncrement(&queue_canwrite);
    if (readers|| writerlock > 0) {
        WaitForSingleObject(can_write_event, INFINITE);
    }

    InterlockedDecrement(&queue_canwrite);
    writerlock = true;
    ResetEvent(can_write_event);
}

//закрытие работы писателя
void stop_write() {
    writerlock = false;
    if (!queue_canwrite) {
        SetEvent(can_read_event);
    } else {
        SetEvent(can_write_event);
    }
}

//запись
DWORD WINAPI writer(LPVOID lpParams) {
    for (int i = 0; i < wrt_cnt; i++) {
        Sleep(100);
        start_write();
        val++;
        printf("writer %ld: %ld\tWRITERS in queue: %ld\tREADERS in queue: %ld\tREADERS: %ld\n", (int) lpParams, val, queue_canwrite, queue_canread, readers);
        stop_write();
    }

    return 0;
}

int main() {
    setbuf(stdout, NULL);
    
    if ((mutex = CreateMutex(NULL, FALSE, NULL)) == NULL) {
        perror("mutex creation error");
        return 1;
    }
    if ((can_read_event = CreateEvent(NULL, FALSE, TRUE, NULL)) == NULL) {
        perror("event creation error");
        return 1;
    }
    if ((can_write_event = CreateEvent(NULL, TRUE, TRUE, NULL)) == NULL) {
        perror("event creation error");
        return 1;
    }
    
    for (int i = 0; i < wn; i++) {
        if ((writers_sore[i] = CreateThread(NULL, 0, writer, (LPVOID) i, 0, NULL)) == NULL) {
            perror("thread creation error");
            return 1;
        }
    }
    
    for (int i = 0; i < rn; i++) {
        if ((readers_store[i] = CreateThread(NULL, 0, reader, (LPVOID) i, 0, NULL)) == NULL) {
            perror("thread creation error");
            return 1;
        }
    }

    WaitForMultipleObjects(wn, writers_sore, TRUE, INFINITE);
    WaitForMultipleObjects(rn, readers_store, TRUE, INFINITE);

    CloseHandle(can_read_event);
    CloseHandle(can_write_event);
    CloseHandle(mutex);

    return 0;
}
