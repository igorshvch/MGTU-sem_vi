#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

WSADATA wsaData;

int main(void) 
{
    int iResult;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    int count = 10;

    SOCKET ListenSocket = CreateListenSocket();

    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;       

    printf("\nListening to a socket at adress 127.0.0.1:27015...\n\n");
    
    while (1)
    {
        int i;
        char *s = "S";
        ZeroMemory(recvbuf, recvbuflen);
        
        iResult = recv(ListenSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            for (i=0;recvbuf[i]!='\0';i++)
                printf("%c", recvbuf[i]);
            convert_and_print(recvbuf);
        }
        
        if (count-- == 0) {
            printf("Shutdown server? For termination press 1\n>>> ");
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

    ShutdownListenSocket(ListenSocket);

    getch();
    return 0;
}

SOCKET CreateListenSocket()
{
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;


    ZeroMemory(&hints, sizeof(hints));
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

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the UDP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    
    return ListenSocket;
}

int ShutdownListenSocket(SOCKET ListenSocket)
{
    int iResult;

    // shutdown the connection since we're done
    iResult = shutdown(ListenSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}

int convert_and_print(char *b)
{
    int n = atoi(b);
    char res2[DEFAULT_BUFLEN] = { 0 };
    convert_int(n, 2, res2);
        
    char res13[DEFAULT_BUFLEN] = { 0 };
    convert_int(n, 13, res13);
                
    printf("decimal: %d\n", n);
    printf("binary: %s\n", res2);
    printf("octal: %o\n", n);
    printf("hexadecimal: %x\n", n);
    printf("13th: %s\n", res13);
    
    return 0;
}

// a - decimal
// p - new base [2, 30]
// s - result string
int convert_int(int a, int p, char *s)
{
    char letters[30] = {"0123456789ABCDEFGHIJKLMNOPQRST"};
    
    int num = (int)a;
    int rest = num % p;
    num /= p;
    if (num == 0)
    {
        s[0] = letters[rest]; 
        return 1;
    }
    int k = convert_int(num, p, s);
    s[k++] = letters[rest];
    return k;
}
