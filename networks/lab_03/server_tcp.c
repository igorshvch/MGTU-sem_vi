#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "httpfuncs/httpfuncs.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 4096
#define DEFAULT_PORT "27015"
#define MAX_THREADS 5

WSADATA wsaData;
HANDLE mutex;


DWORD WINAPI client_thread(LPVOID lpParams)
{
    int iResult;
    int iSendResult;
    int pID = GetCurrentThreadId();
    

    SOCKET client = (SOCKET)lpParams;

    unsigned long sendbuflen = 4194304;
    unsigned long data_length;
    int recvbuflen = 4096;
    char *sendbuf = (char*)malloc(sizeof(char)*sendbuflen);
    char *recvbuf = (char*)malloc(sizeof(char)*recvbuflen);

    ZeroMemory(sendbuf, sizeof(char)*sendbuflen);
    ZeroMemory(recvbuf, sizeof(char)*recvbuflen);

    //reciving data
    printf(">>> reciving...\n");
    iResult = recv(client, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("pID %d>>> Bytes received: %d\n", pID, iResult);
        }

    char temp[DEFAULT_BUFLEN];
    char method[10];
    char file_name[DEFAULT_BUFLEN];
    strcpy(file_name, "None");
    
    strcpy(temp, recvbuf);
    printf("pID %d>>> ===>REQUEST HEADER: %s\n", pID,strtok(temp, "\r\n"));
    request_parser(temp, method, file_name);
    printf("pID %d>>> ===>method: '%s', file: '%s'\n", pID, method, file_name);

    if (strcmp(method, "GET") == 0) {
        if (strcmp(file_name, "None") == 0) {
            get_redirect(sendbuf, &data_length);
        } else if (strstr(file_name, "html") != NULL) {
            get_index(sendbuf, "static_files/index.html", &data_length);
        } else if (strstr(file_name, "jpg") != NULL) {
            get_image(sendbuf,"static_files/universe.jpg", &data_length);
        } else if ((strstr(file_name, "ico") != NULL)){
            get_ico(sendbuf,"static_files/favicon.ico", &data_length);    
        }
    } else if ((strcmp(method, "HEAD") == 0)) {
        if (strcmp(file_name, "None") == 0) {
            head_redirect(sendbuf, &data_length);
        } else if (strstr(file_name, "html") != NULL) {
            head_index(sendbuf, "static_files/index.html", &data_length);
        } else if (strstr(file_name, "jpg") != NULL) {
            head_image(sendbuf,"static_files/universe.jpg", &data_length);
        } else if ((strstr(file_name, "ico") != NULL)){
            head_ico(sendbuf,"static_files/favicon.ico", &data_length);    
        }
    } else {
            printf("pID %d>>> Incorrect http-request <<<\n", pID);
            strcpy(sendbuf, "HTTP/1.1 501 Not Implemented\r\n");
            data_length = (unsigned long)strlen(sendbuf);
        }

    strcpy(temp, sendbuf);
    printf("pID %d>>> ===>RESPONSE HEADER: %s\n", pID, strtok(temp, "\r\n"));

    //sending data
    printf("pID %d>>> sending...\n", pID);
    iSendResult = send( client, sendbuf, data_length, 0 );
    if (iSendResult == SOCKET_ERROR) {
        printf("pID %d>>> send failed with error: %d\n", pID, WSAGetLastError());
        closesocket(client);
        WSACleanup();
        return 1;
    }
    printf("pID %d>>> Bytes sent: %d\n", pID, iSendResult);
    

    // shutdown the connection since we're done
    iResult = shutdown(client, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("pID %d>>> shutdown failed with error: %d\n",pID, WSAGetLastError());
        closesocket(client);
        WSACleanup();
        return 1;
    }

    free(sendbuf);
    free(recvbuf);

    ExitThread(0);

    return 0;
}

int main(void) 
{
    setbuf(stdout, NULL);

    if ((mutex = CreateMutex(NULL, FALSE, NULL)) == NULL) {
        perror("mutex creation error");
        return 1;
    }

    int iResult;
    int ACTIVE_THREADS = 0;

    HANDLE threads_in_process[MAX_THREADS];

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;
  
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    
    // Resolve the server address and port
    iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    
    freeaddrinfo(result);

    printf("\n\t>>> TCP SERVER: Listening to a socket at adress 127.0.0.1:27015... <<<\n\n");
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf(">>> Ready to accept connetcion...\n");
    while (1)
    {
        // Accept a client socket
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        printf(">>> Connection accepted!\n");

        if (ACTIVE_THREADS < MAX_THREADS) {
            threads_in_process[ACTIVE_THREADS] = CreateThread(NULL, 0, client_thread, (LPVOID)ClientSocket, 0, NULL);
            ACTIVE_THREADS++;
            printf(">>> WARNING! currently active connections: %d\n", ACTIVE_THREADS);
        } else {
            WaitForMultipleObjects(MAX_THREADS, threads_in_process, 1, INFINITE);
            ACTIVE_THREADS=0;
        }
    }


    // cleanup
    for (int i =0; i < MAX_THREADS; i++) {
        CloseHandle(threads_in_process[i]);
    }
    CloseHandle(mutex);
    closesocket(ClientSocket);
    closesocket(ListenSocket);
    WSACleanup();

    printf("\n\t>>> TCP SERVER successfully terminated<<<\n\n");
    return 0;
}