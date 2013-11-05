

/*
 * Sample Codes for Network Programming Class (2013)
 *
 * Chaeseok, Lee (quarry@mobile.re.kr)
 * 
 * KyungSung University 
 *  
 */

// DatagramServer.c
// A Simple UDP Server receiving some messages from clients

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define	APP_RCV_BUF_SIZE	100

void ErrorHandling(char *message);

int main(int argc, char* argv[])
{
	int sock;		
	struct sockaddr_in servAddr, clntAddr;	
	char app_rBuffer[APP_RCV_BUF_SIZE+1];

/* 0 */
	if(argc!=2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
/* 1 */
	if ( (sock=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		ErrorHandling("socket() error");

/* 2 */	
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAddr.sin_port=htons(atoi(argv[1]));
	if( bind(sock, (struct sockaddr*) &servAddr, sizeof(servAddr))== -1 ) 
		ErrorHandling("bind() error");  

/* 3 */
	int nReceived, nAddrLen = sizeof(clntAddr);
	do
	{
		if ((nReceived=recvfrom(sock, app_rBuffer, APP_RCV_BUF_SIZE, 0, (struct sockaddr *)&clntAddr, &nAddrLen)) != -1)
		{
			app_rBuffer[nReceived]=0;
			printf("[From %s:%d][Size %d]-\"%.40s...\"\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port),nReceived, app_rBuffer);  
		}
		else
		{
			printf("[Error]\n");
		}
	} while (nReceived > 0);

/* 4 */
	close(sock);	
	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

