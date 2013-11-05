

/*
 * Sample Codes for Network Programming Class (2013)
 *
 * Chaeseok, Lee (quarry@mobile.re.kr)
 * 
 * KyungSung University 
 *  
 */

// StreamServer_PressEnter.C
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define	APP_RCV_BUF_SIZE	100
#define SYS_RCV_BUF_SIZE	100

void ErrorHandling(char *message);
int mygetch(void);

int main(int argc, char* argv[])
{
	int		hListenSock, hSock;		
	struct sockaddr_in servAddr, clntAddr;		

	char		app_rBuffer[APP_RCV_BUF_SIZE+1];

/* 1 */
	if(argc!=2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
 
/* 2 */
	if ( (hListenSock=socket(AF_INET, SOCK_STREAM, 0)) == -1)
		ErrorHandling("socket() error");

/* 3 */	
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family			=AF_INET;
	servAddr.sin_addr.s_addr	=htonl(INADDR_ANY);
	servAddr.sin_port			=htons(atoi(argv[1]));
	if( bind(hListenSock, (struct sockaddr*)&servAddr, sizeof(servAddr))== -1 ) 
		ErrorHandling("bind() error");  

/* 4 */
	if( listen(hListenSock, 5)==-1) 
		ErrorHandling("listen() error");

/* 5 */
	while (1)
	{
		int nReceived, nAddrLen = sizeof(clntAddr);

/* 6 */
		if ( (hSock=accept(hListenSock, (struct sockaddr*)&clntAddr,&nAddrLen)) == -1)
			ErrorHandling("accept() error");  

/* 7 */
		int nBufSize = SYS_RCV_BUF_SIZE, nLen=sizeof(int);
		if (setsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char *) &nBufSize, sizeof(nBufSize))==-1)
			printf("Set Socket Option Error !\n");
		if (getsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char *) &nBufSize, &nLen)==-1)
			printf("Get Socket Option Error !\n");
		printf("[APP/SYS] Receiving Buffer Size is Set to [%d/%d] Bytes\n", APP_RCV_BUF_SIZE, nBufSize);

/* 8 */
		do
		{
			printf("Press Enter to retrieve data from Socket\n");
			if (mygetch() == '\r') break;
			fflush(stdin);

			if ((nReceived=recv(hSock, app_rBuffer, APP_RCV_BUF_SIZE, 0)) != -1)
			{
				app_rBuffer[nReceived]=0;
				printf("[From %s:%d][Size %d]-\"%.40s...\"\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port),nReceived, app_rBuffer);  
			}
			else
			{
				printf("[Error] Receive Error\n");
			}
		} while (nReceived > 0);

/* 7 */
		close(hSock);	

/* 8 */
		printf("Press Enter to accept a new connection request\n");
		if (mygetch() == '\r') break;
		fflush(stdin);
	}

/* 9 */
	close(hListenSock);

	return 0;
}

int mygetch(void)
{
	struct termios oldt,newt;
	int ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}


void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
