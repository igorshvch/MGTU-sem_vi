#include <stdio.h>
#include <stdbool.h>
#include <windows.h>

#define rn 5
#define wn 3
#define rd_cnt 3
#define wrt_cnt 4

HANDLE writers[wn];
HANDLE readers[rn];

HANDLE mutex;
HANDLE can_read;      //событие, пропускающее читателей
HANDLE can_write;     //событие, пропускающее писателей

volatile int val = 0; //переменная, разделяемая между потоками
                      //в нее происходит запись и чтение

volatile LONG wr = 0;
volatile LONG ww = 0;
volatile LONG ar = 0;
bool writerlock = false;

//откртие работы читателя
void start_read() {
    InterlockedIncrement(&wr);
    if (writerlock || WaitForSingleObject(can_write, 0) == WAIT_OBJECT_0) {
        WaitForSingleObject(can_read, INFINITE);
    }
    
    WaitForSingleObject(mutex, INFINITE);
    
    InterlockedDecrement(&wr);
    InterlockedIncrement(&ar);
    SetEvent(can_read);
    
    ReleaseMutex(mutex);
}

//закрытие работы читателя
void stop_read() {
    InterlockedDecrement(&ar);
    if (wr == 0) {
        SetEvent(can_write);
    }
}

//чтение
DWORD WINAPI reader(LPVOID lpParams) {
    for (int i = 0; i < rd_cnt; i++) {
        Sleep(100);
        start_read();
        printf("reader %ld: %ld\n", (int) lpParams, val);
        stop_read();
    }
    
    return 0;
}

//откртие работы писателя
void start_write() {
    InterlockedIncrement(&ww);
    if (ar || writerlock > 0) {
        WaitForSingleObject(can_write, INFINITE);
    }

    InterlockedDecrement(&ww);
    writerlock = true;
    ResetEvent(can_write);
}

//закрытие работы писателя
void stop_write() {
    writerlock = false;
    if (!ww) {
        SetEvent(can_read);
    } else {
        SetEvent(can_write);
    }
}

//запись
DWORD WINAPI writer(LPVOID lpParams) {
    for (int i = 0; i < wrt_cnt; i++) {
        Sleep(100);
        start_write();
        val++;
        printf("writer %ld: %ld\n", (int) lpParams, val);
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
    if ((can_read = CreateEvent(NULL, FALSE, TRUE, NULL)) == NULL) {
        perror("event creation error");
        return 1;
    }
    if ((can_write = CreateEvent(NULL, TRUE, TRUE, NULL)) == NULL) {
        perror("event creation error");
        return 1;
    }
    
    for (int i = 0; i < wn; i++) {
        if ((writers[i] = CreateThread(NULL, 0, writer, (LPVOID) i, 0, NULL)) == NULL) {
            perror("thread creation error");
            return 1;
        }
    }
    
    for (int i = 0; i < rn; i++) {
        if ((readers[i] = CreateThread(NULL, 0, reader, (LPVOID) i, 0, NULL)) == NULL) {
            perror("thread creation error");
            return 1;
        }
    }

    WaitForMultipleObjects(wn, writers, TRUE, INFINITE);
    WaitForMultipleObjects(rn, readers, TRUE, INFINITE);

    CloseHandle(can_read);
    CloseHandle(can_write);
    CloseHandle(mutex);

    return 0;
}
