#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
#pragma comment(lib, "Ws2_32.lib")
 
char html[] =
    "HTTP/1.0 200 OK\r\n"
    "Connection: close\r\n"
    "\r\n";
 
char document[2048];
char *buffer;
long lSize;
int documentPosition = 0;
 
WSADATA wsaData;
SOCKET sock;
int tcpPort = 60000;
int verbose = 1;
 
void sendRespond(SOCKET clientSock)
{
    char *tmp = new char[lSize+strlen(html)];
 
    int intlt = 0;
    for(; intlt < strlen(html); intlt++)
        tmp[intlt] = html[intlt];
 
    for(int i = 0; intlt < strlen(html) + lSize; intlt++, i++)
        tmp[intlt] = buffer[i];
 
    send(clientSock, tmp, intlt, 0);
}
 
bool loadFile(char* file)
{
    if(file[2] == 0)
        _snprintf(file, strlen(".\\index.html"), "%s", ".\\index.html");
    FILE *ptrFile = fopen(file, "rb");
    if(ptrFile == NULL)
        return false;
 
    fseek(ptrFile, 0, SEEK_END);
    lSize = ftell(ptrFile);
    rewind(ptrFile);
 
    buffer = new char[lSize];
    fread(buffer, sizeof(char), lSize, ptrFile);
    fclose(ptrFile);
    return true;
}
 
void carp(wchar_t* msg)
{
    fwprintf(stderr, L"%s\n", msg);
}
 
void exitService(wchar_t* msg)
{
    carp(msg);
    WSACleanup();
    exit(1);
}
 
int handleClient(SOCKET clientSock)
{
    char cCur = 0, cLast = 0, cBuf = 0;
    bool headersEnded = false;
    char reqHdr[1024] = { 0 };
    int reqHdrLen = 0;
 
    // Wait until we've actually received the end of the HTTP request headers
    // before we start sending our response...
    do {
        int count = recv(clientSock, &cBuf, 1, 0);
        if ( count == SOCKET_ERROR ) {
            carp(L"Could not recv from client");
            return 1;
        }
 
        if ( cBuf != '\r' && cBuf != '\b' ) {
            cLast = cCur;
            cCur = cBuf;
            if ( reqHdrLen < 1024 )
                reqHdr[reqHdrLen++] = cBuf;
        }
 
        if ( cCur == '\n' && cLast == '\n' )
            headersEnded = true;
    } while ( ! headersEnded );
 
    if ( verbose )
        printf("\n%s", reqHdr);
 
 
    char get[1024] = { 0 };
    int cursor = 0, target = 0;
    while(reqHdr[cursor++] != ' '){}
    get[target++] = '.';
    get[target++] = '\\';
    cursor++;
    while(reqHdr[cursor] != ' ')
        get[target++] = reqHdr[cursor++];
    if(strstr(get,".\\error"))
    {
        char* ptr = "HTTP/1.1 503 Service Temporarily Unavailable\r\nConnection: close\r\n\r\nSERVER PUT DOWN";
        send(clientSock,ptr,strlen(ptr),0);
 
        return 0;
    }
 
    ;
    if(loadFile(get))
        sendRespond(clientSock);
    else
    {
        char* ptr = "HTTP/1.1 404 File Not Found\r\nConnection: close\r\n\r\nNe NASHOL!";
        send(clientSock,ptr,strlen(ptr),0);
    }
    return 0;
}
 
int main(int argc, char* argv[])
{
    SOCKADDR_IN sockAddr;
    SOCKET clientSock;
    int i;
    if ( WSAStartup(MAKEWORD(2,2), &wsaData) != 0 )
        exitService(L"Winsock2 initialization failed");
 
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if ( sock == INVALID_SOCKET )
        exitService(L"Socket creation failed");
 
    sockAddr.sin_port = htons((u_short)tcpPort);
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
 
    if ( bind(sock, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR )
        exitService(L"Unable to bind TCP socket on this port");
 
    if ( listen(sock, 1000) == SOCKET_ERROR )
        exitService(L"Unable to listen");
 
    wprintf(L"Listening on port %d...\n", tcpPort);
 
    while ( clientSock = accept(sock, NULL, 0) ) 
    {
        if ( clientSock == INVALID_SOCKET ) 
        {
            closesocket(clientSock);
            exitService(L"Accept failed");
        }
        handleClient(clientSock);
        shutdown(clientSock, SD_BOTH);
        closesocket(clientSock);
    }
 
    return 0;
}