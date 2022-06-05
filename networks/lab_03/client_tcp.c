#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 4096
#define DEFAULT_PORT "27015"

int main() 
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

    char sendbuf[DEFAULT_BUFLEN];
    char recvbuf[DEFAULT_BUFLEN];
    char *file_path;
    int iResult;
    int sendbuflen = DEFAULT_BUFLEN;
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

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

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    
    printf("\n\t\t>>> TCP CLIENT: Connected to the server at 127.0.0.1:%s <<<\n\n", DEFAULT_PORT);

    ZeroMemory(recvbuf, recvbuflen);
    ZeroMemory(sendbuf, sendbuflen);  

    printf("Choose line number with requset type:\n");
    printf("1. GET to starter page\n");
    printf("2. HEAD to starter page\n");
    printf("3. HEAD to image\n");
    printf("4. HEAD to page icon\n");
    char c = getch();
    
    if (c == '1')
        strcpy(sendbuf, "GET /index.html HTTP/1.1\r\n");
    else if (c == '2')
        strcpy(sendbuf, "HEAD /index.html HTTP/1.1\r\n");
    else if (c == '3')
        strcpy(sendbuf, "HEAD /universe.jpg HTTP/1.1\r\n");
    else if (c == '4')
        strcpy(sendbuf, "HEAD /favicon.ico HTTP/1.1\r\n");
    else {
        printf(">>> Incorrect inputn!\n");
    }

    //sending
    iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    printf(">>> Bytes Sent: %ld\n", iResult);

    ZeroMemory(recvbuf, recvbuflen);
    ZeroMemory(sendbuf, sendbuflen);

    //reciving
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if ( iResult > 0 )
        printf(">>> Bytes received: %d\n", iResult);
    else if ( iResult == 0 )
        printf("Connection closed\n");
    else
        printf("recv failed with error: %d\n", WSAGetLastError());
    
    printf(">>> Data recived successfully!\n");
    printf(">>> ==>RESPONSE CONTENT:\n%s", recvbuf);

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