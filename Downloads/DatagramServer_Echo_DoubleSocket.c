
/*
 * Sample Codes for Network Programming Class (2013)
 *
 * Chaeseok, Lee (quarry@mobile.re.kr)
 * 
 * KyungSung University 
 *  
 */

// DatagramServer_Echo_DoubleSocket.c : Defines the entry point for the console application.
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

//#define	NON_BLOCKING_IO		

void ErrorHandling(char *message);

int main(int argc, char* argv[])
{
	int		hSock1, hSock2;		
	struct sockaddr_in servAddr, clntAddr;	
	char		app_rBuffer[APP_RCV_BUF_SIZE+1];

	if(argc!=2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

/* 1 */
	if ( (hSock1=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		ErrorHandling("socket() error");
	if ( (hSock2=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		ErrorHandling("socket() error");

/* 2 */	
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family			=AF_INET;
	servAddr.sin_addr.s_addr	=htonl(INADDR_ANY);
	servAddr.sin_port			=htons(atoi(argv[1]));
	if( bind(hSock1, (struct sockaddr*)&servAddr, sizeof(servAddr))==-1 ) 
		ErrorHandling("bind() error");  
	servAddr.sin_port			=htons(atoi(argv[1])+1);
	if( bind(hSock2, (struct sockaddr*)&servAddr, sizeof(servAddr))==-1 ) 
		ErrorHandling("bind() error");  

/* 3 */
	int nBufSize = SYS_RCV_BUF_SIZE, nLen=sizeof(int);
	setsockopt(hSock1, SOL_SOCKET, SO_RCVBUF, (char *) &nBufSize, sizeof(nBufSize));
	getsockopt(hSock1, SOL_SOCKET, SO_RCVBUF, (char *) &nBufSize, &nLen);
	printf("[APP/SYS] Receiving Buffer Size of the 1st Socket is Set to [%d/%d] Bytes\n", APP_RCV_BUF_SIZE, nBufSize);
	
	nBufSize = SYS_RCV_BUF_SIZE, nLen = sizeof(int);
	setsockopt(hSock2, SOL_SOCKET, SO_RCVBUF, (char *) &nBufSize, sizeof(nBufSize));
	getsockopt(hSock2, SOL_SOCKET, SO_RCVBUF, (char *) &nBufSize, &nLen);
	printf("[APP/SYS] Receiving Buffer Size of the 2st Socket is Set to [%d/%d] Bytes\n", APP_RCV_BUF_SIZE, nBufSize);

/* 4 */
#ifdef NON_BLOCKING_IO
	int flag;
	if ((flag = fcntl(hSock1, F_GETFL, 0)) == -1){
		printf("Could not change the I/O Mode of the 1st Socket\n");
		flag = 0;
	}
	else {
		fcntl(hSock1, F_SETFL, flag|O_NONBLOCK);	
	}

	if ((flag = fcntl(hSock2, F_GETFL, 0)) == -1){
		printf("Could not change the I/O Mode of the 2nd Socket\n");
		flag = 0;
	}
	else {
		fcntl(hSock2, F_SETFL, flag|O_NONBLOCK);	
	}
#endif

/* 5 */
	int nSent, nReceived, nAddrLen = sizeof(clntAddr), nErrorCode;
	do
	{
		if ((nReceived=recvfrom(hSock1, app_rBuffer, APP_RCV_BUF_SIZE, 0, (struct sockaddr *)&clntAddr, &nAddrLen)) != -1)
		{
			if ((nSent = sendto(hSock1, app_rBuffer, nReceived, 0, (struct sockaddr *)&clntAddr, nAddrLen)) != -1)
			{
				app_rBuffer[nSent]=0;
				printf("[Echo %s:%d][Size %d]-\"%.40s...\"\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port),nSent, app_rBuffer);  
			}
			else
			{
				nErrorCode = 1;
			}
		}
		else
		{
			nErrorCode = 1;
		}

		if ((nReceived=recvfrom(hSock2, app_rBuffer, APP_RCV_BUF_SIZE, 0, (struct sockaddr *)&clntAddr, &nAddrLen))  != -1)
		{
			if ((nSent = sendto(hSock2, app_rBuffer, nReceived, 0, (struct sockaddr *)&clntAddr, nAddrLen))  != -1)
			{
				app_rBuffer[nSent]=0;
				printf("[Echo %s:%d][Size %d]-\"%.40s...\"\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port),nSent, app_rBuffer);  
			}
			else
			{
				nErrorCode = 1;
			}
		}
		else
		{
			nErrorCode = 1;
		}

	} while (nReceived > 0 || nErrorCode == 1);

/* 6 */
	close(hSock1);	
	close(hSock2);

	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}


