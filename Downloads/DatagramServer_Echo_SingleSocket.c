

/*
 * Sample Codes for Network Programming Class (2013)
 *
 * Chaeseok, Lee (quarry@mobile.re.kr)
 * 
 * KyungSung University 
 *  
 */

// DatagramServer_Echo_SingleSocket.c : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>

#define	APP_RCV_BUF_SIZE	100
#define SYS_RCV_BUF_SIZE	100
#define SYS_SND_BUF_SIZE	100

void ErrorHandling(char *message);

int main(int argc, char* argv[])
{
	int		hSock;	
	struct sockaddr_in servAddr, clntAddr;	
	char		app_rBuffer[APP_RCV_BUF_SIZE+1];

	if(argc!=2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
/* 1 */
	if ( (hSock=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		ErrorHandling("socket() error");

/* 2 */	
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family			=AF_INET;
	servAddr.sin_addr.s_addr	=htonl(INADDR_ANY);
	servAddr.sin_port			=htons(atoi(argv[1]));
	if( bind(hSock, (struct sockaddr*)&servAddr, sizeof(servAddr))==-1 ) 
		ErrorHandling("bind() error");  

/* 3 */
	int nBufSize = SYS_RCV_BUF_SIZE, nLen=sizeof(int);
	setsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char *) &nBufSize, sizeof(nBufSize));
	getsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char *) &nBufSize, &nLen);
	printf("[APP/SYS] Receiving Buffer Size is Set to [%d/%d] Bytes\n", APP_RCV_BUF_SIZE, nBufSize);
	

/* 4 */
	int nSent, nReceived, nAddrLen = sizeof(clntAddr), nErrorCode;
	do
	{
		if ((nReceived=recvfrom(hSock, app_rBuffer, APP_RCV_BUF_SIZE, 0, (struct sockaddr *)&clntAddr, &nAddrLen)) != -1)
		{
			if ((nSent = sendto(hSock, app_rBuffer, nReceived, 0, (struct sockaddr *)&clntAddr, nAddrLen)) != -1)
			{
				app_rBuffer[nSent]=0;
				printf("[Echo %s:%d][Size %d]-\"%.40s...\"\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port),nSent, app_rBuffer);  
			}
			else
			{
				nErrorCode = -1;
			}
		}
		else
		{
			nErrorCode = -1;
		}

	} while (nReceived > 0 || nErrorCode == -1);

/* 5 */
	close(hSock);	

	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}


