#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <windows.h>
#include <map>
#include <time.h>

HANDLE mutex;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int sockfd, portno, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char buffer[256];

DWORD WINAPI ReadThread(void* args)
{
	while(1)
	{
	if (WAIT_OBJECT_0 == WaitForSingleObject(mutex, INFINITE)) 
		{
			n = read(sockfd, buffer, 255);

			if (n < 0) 
				error("ERROR reading from socket");

			char *ctime_no_newline;
			time_t tm = time(NULL);

			ctime_no_newline = strtok(ctime(&tm), "\n");
			printf("Otrzymano wiadomość %s", ctime_no_newline);
			printf(" - %.*s", n - 1, buffer);
			ReleaseMutex(mutex);
		}
	}
	return 1;
}

DWORD WINAPI WriteThread(void* args)
{
	while(1)
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(mutex, INFINITE)) 
		{
			bzero(buffer,256);
			fgets(buffer,255,stdin);
			n = write(sockfd, buffer, strlen(buffer));

			if (n < 0) 
				error("ERROR writing to socket");

			bzero(buffer,256);
			ReleaseMutex(mutex);
		}
	}
	return 1;
}

int main(int argc, char *argv[])
{
HANDLE thread1, thread2;
mutex = CreateMutex(nullptr, FALSE, nullptr);

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    
	portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
	if (sockfd < 0) 
        error("ERROR opening socket");
    
	server = gethostbyname(argv[1]);
    
	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
	bzero((char *) &serv_addr, sizeof(serv_addr));
	
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
		 
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
	
	thread1 = CreateThread(nullptr, 0, ReadThread, nullptr, 0 , nullptr);
	thread2 = CreateThread(nullptr, 0, WriteThread, nullptr, 0 , nullptr);
	
	while(1){}
}