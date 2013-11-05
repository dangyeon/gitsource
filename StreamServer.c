

/*
 * Sample Codes for Network Programming Class (2013)
 *
 * Chaeseok, Lee (quarry@mobile.re.kr)
 * 
 * KyungSung University 
 *  
 */

// StreamServer.c
// A Simple TCP Server sending a few messages to clients

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define	APP_RCV_BUF_SIZE	100

A
A

void ErrorHandling(char *message);

int main(int argc, char* argv[])
{
	int ListenSock, Sock;		
	struct sockaddr_in servAddr, clntAddr;	
	int nAddrLen = sizeof(clntAddr);

	char app_rBuffer[APP_RCV_BUF_SIZE+1];

/* 0 */
	if(argc!=2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
 
/* 1 */
	if ( (ListenSock=socket(AF_INET, SOCK_STREAM, 0)) == -1)
		ErrorHandling("socket() error");

/* 2 */	
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAddr.sin_port=htons(atoi(argv[1]));

	if( bind(ListenSock, (struct sockaddr*) &servAddr, sizeof(servAddr))== -1 ) 
		ErrorHandling("bind() error");  

/* 3 */
	if( listen(ListenSock, 5) == -1) 
		ErrorHandling("listen() error");

/* 4 */
	while (1)
	{
/* 5 */
		if ( (Sock=accept(ListenSock, (struct sockaddr*)&clntAddr,&nAddrLen)) == -1)
			ErrorHandling("accept() error");  

/* 6 */
		int nReceived;
		do
		{
			if ((nReceived=recv(Sock, app_rBuffer, APP_RCV_BUF_SIZE, 0)) != -1)
			{
				app_rBuffer[nReceived]=0;
				printf("[From %s:%d][Size %d]-\"%.40s...\"\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port),nReceived, app_rBuffer);  
			}
			else 
			{
				printf("[Error]\n");
			}
		} while (nReceived > 0);

/* 7 */
		close(Sock);	
	}

/* 8 */
	close(ListenSock);

	return 0;
}


void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
