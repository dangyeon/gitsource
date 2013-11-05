
/*
 * Sample Codes for Network Programming Class (2013)
 *
 * Chaeseok, Lee (quarry@mobile.re.kr)
 * 
 * KyungSung University 
 *  
 */

// StreamServer_Echo_DoubleClient.c : Defines the entry point for the console application.
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

#define	NUM_CLIENT			2

#define	NON_BLOCKING_IO		

void ErrorHandling(char *message);
inline int HandleSocketError(int bPrintBlockError);

int main(int argc, char* argv[])
{
	int	hListenSock, hSock[NUM_CLIENT];		
	struct sockaddr_in servAddr, clntAddr;	
	char		app_rBuffer[APP_RCV_BUF_SIZE+1];
	int			nAddrLen=sizeof(clntAddr), nClient=0;

	if(argc!=2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
 
/* 1 */
	if ( (hListenSock=socket(AF_INET, SOCK_STREAM, 0)) == -1)
		ErrorHandling("socket() error");

/* 2 */	
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family			=AF_INET;
	servAddr.sin_addr.s_addr	=htonl(INADDR_ANY);
	servAddr.sin_port			=htons(atoi(argv[1]));
	if( bind(hListenSock, (struct sockaddr*) &servAddr, sizeof(servAddr))== -1 ) 
		ErrorHandling("bind() error");  

	if( listen(hListenSock, 5)== -1) 
		ErrorHandling("listen() error");


/* 3 */
	for (nClient=0; nClient < NUM_CLIENT; nClient++)
	{
		if ( (hSock[nClient]=accept(hListenSock, (struct sockaddr*)&clntAddr,&nAddrLen)) == -1)
			ErrorHandling("accept() error");  

		int nBufSize = SYS_RCV_BUF_SIZE, nLen=sizeof(int);
		setsockopt(hSock[nClient], SOL_SOCKET, SO_RCVBUF, (char *) &nBufSize, sizeof(nBufSize));
		getsockopt(hSock[nClient], SOL_SOCKET, SO_RCVBUF, (char *) &nBufSize, &nLen);
		printf("[APP/SYS] Receiving Buffer Size of the %dth Socket is Set to [%d/%d] Bytes\n", nClient, APP_RCV_BUF_SIZE, nBufSize);

/* 4 */
#ifdef NON_BLOCKING_IO
		int flag;
		if ((flag = fcntl(hSock[nClient], F_GETFL, 0)) == -1){
			printf("Could not change the I/O Mode of the %d Socket\n",nClient);
			flag = 0;
		}
		else {
			fcntl(hSock[nClient], F_SETFL, flag|O_NONBLOCK);	
		}	
#endif
	}

/* 5 */
	int nSent, nReceived, nErrorCode;
	do
	{
		for (nClient=0; nClient < NUM_CLIENT; nClient++)
		{
			if ((nReceived=recv(hSock[nClient], app_rBuffer, APP_RCV_BUF_SIZE, 0)) != -1)
			{
				if ((nSent = send(hSock[nClient], app_rBuffer, nReceived, 0)) != -1)
				{
					app_rBuffer[nSent]=0;
					printf("[Echo %d]-\"%.40s...\"\n", nSent, app_rBuffer);  
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
		}

	} while (nReceived > 0 || nErrorCode == -1);

/* 6 */
	for (nClient=0; nClient < NUM_CLIENT; nClient++)
		close(hSock[nClient]);

	close(hListenSock);

	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
