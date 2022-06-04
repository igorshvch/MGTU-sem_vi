#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

WSADATA wsaData;

int main() 
{
    int iResult;
    int count = 10;

    SOCKET ConnectSocket = INVALID_SOCKET;
    
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

     struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    // Resolve the server address and port
    iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
    }

    putchar('\n');
    
    while (1)
    {
        ZeroMemory(recvbuf, recvbuflen);
        printf("Input message: ");
        fgets(recvbuf, recvbuflen, stdin);

        // Send an initial buffer
        iResult = sendto( ConnectSocket, recvbuf, (int)strlen(recvbuf), 0, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }

        printf("Bytes Sent: %ld\n", iResult);

        if (count-- == 0) {
            printf("Shutdown client? For termination press 1\n>>> ");
            char c = getch();
            putchar(c);
            putchar('\n');
            if (c == '1')
                break;
            count = 10;
        } else
            printf("%d attempts before termination proposal\n", count);
        printf("=========================================\n");
    }


    freeaddrinfo(result);
    ShutdownConnectSocket(ConnectSocket);

    getch();
    return 0;
}

int ShutdownConnectSocket(SOCKET ConnectSocket)
{
    int iResult = 0;

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}