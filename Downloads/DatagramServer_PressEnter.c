/*
 * Sample Codes for Network Programming Class (2013)
 *
 * Chaeseok, Lee (quarry@mobile.re.kr)
 * 
 * KyungSung University 
 *  
 */

// DatagramServer_PressEnter.c 
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define	APP_RCV_BUF_SIZE	100
#define SYS_RCV_BUF_SIZE	100

void ErrorHandling(char *message);
int mygetch(void);

int main(int argc, char* argv[])
{
	int	hSock;		
	struct sockaddr_in servAddr, clntAddr;	
	char		app_rBuffer[APP_RCV_BUF_SIZE+1];

/* 1 */
	if(argc!=2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
 
/* 2 */
	if ( (hSock=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		ErrorHandling("socket() error");

/* 3 */	
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family			=AF_INET;
	servAddr.sin_addr.s_addr	=htonl(INADDR_ANY);
	servAddr.sin_port			=htons(atoi(argv[1]));
	if( bind(hSock, (struct sockaddr*) &servAddr, sizeof(servAddr))== -1 ) 
		ErrorHandling("bind() error");  

/* 4 */
	int nBufSize = SYS_RCV_BUF_SIZE, nLen=sizeof(int);
	setsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char *) &nBufSize, sizeof(nBufSize));
	getsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char *) &nBufSize, &nLen);
	printf("[APP/SYS] Receiving Buffer Size is Set to [%d/%d] Bytes\n", APP_RCV_BUF_SIZE, nBufSize);

/* 5 */
	int nReceived, nAddrLen = sizeof(clntAddr);
	do
	{
		printf("Press Enter to retrieve data from Socket\n");
		if (mygetch() == '\r') break;
		fflush(stdin);

		if ((nReceived=recvfrom(hSock, app_rBuffer, APP_RCV_BUF_SIZE, 0, (struct sockaddr *)&clntAddr, &nAddrLen)) != -1)
		{
			app_rBuffer[nReceived]=0;
			printf("[From %s:%d][Size %d]-\"%.40s...\"\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port),nReceived, app_rBuffer);  
		}
		else
		{
			printf("[Error] Receive Error\n");
		}
	} while (nReceived > 0);

/* 6 */
	close(hSock);	

	return 0;
}

void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
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

