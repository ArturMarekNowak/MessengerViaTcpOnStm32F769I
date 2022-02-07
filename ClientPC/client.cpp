#include <stdio.h>
#include <winsock.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <windows.h>
#include <unistd.h>
#include <map>
#include <chrono>

#define bzero(b, len) (memset((b), '\0', (len)), (void)0)
#define bcopy(b1, b2, len) (memmove((b2), (b1), (len)), (void)0)

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int sockfd, portno, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char buffer[256];

DWORD WINAPI ReadThread(void *args)
{
    while (1)
    {
        n = recv(sockfd, buffer, 255, 0);
        if (n < 0)
            error("ERROR reading from socket");
        printf("%s\n", buffer);
    }
    return 1;
}

DWORD WINAPI WriteThread(void *args)
{
    while (1)
    {
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        n = send(sockfd, buffer, strlen(buffer), 0);
        if (n < 0)
            error("ERROR writing to socket");
        bzero(buffer, 256);
    }
    return 1;
}

int main(int argc, char *argv[])
{
    WSADATA Data;
    HANDLE thread1, thread2;
    WSAStartup(MAKEWORD(2, 2), &Data);

    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);

    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    thread1 = CreateThread(nullptr, 0, ReadThread, nullptr, 0, nullptr);
    thread2 = CreateThread(nullptr, 0, WriteThread, nullptr, 0, nullptr);

    while (1)
    {
    }
}
