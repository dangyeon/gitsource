

/*
 * Sample Codes for Network Programming Class (2013)
 *
 * Chaeseok, Lee (quarry@mobile.re.kr)
 * 
 * KyungSung University 
 *  
 */

// StreamClient_Echo.c : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>

#define	APP_SND_BUF_SIZE	20
#define SYS_SND_BUF_SIZE	100

void ErrorHandling(char *message);
inline int GetUserMessage(int i, char * buf);
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

int main(int argc, char* argv[])
{
	int		hSock;		
	struct sockaddr_in servAddr;	

	char		app_wBuffer[APP_SND_BUF_SIZE+1];

	if(argc!=3)
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

/* 1 */ 	
	if ( (hSock=socket(AF_INET, SOCK_STREAM, 0)) == -1)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family			=AF_INET;
	servAddr.sin_addr.s_addr	=inet_addr(argv[1]);
	servAddr.sin_port			=htons(atoi(argv[2]));

	if( connect(hSock, (struct sockaddr*) &servAddr, sizeof(servAddr))==-1 ) 
		ErrorHandling("connect() error!");


/* 2 */ 
	int nBufSize = SYS_SND_BUF_SIZE, nLen=sizeof(int);
	setsockopt(hSock, SOL_SOCKET, SO_SNDBUF, (char *) &nBufSize, sizeof(nBufSize));
	getsockopt(hSock, SOL_SOCKET, SO_SNDBUF, (char *) &nBufSize, &nLen);
	printf("[APP/SYS] Sending Buffer Size is Set to [%d/%d] Bytes\n", APP_SND_BUF_SIZE, nBufSize);

/* 3 */ 
	int nMsgSize, nSent, nReceived,i;
	for (i=1; i < 10000; i++)
	{
		nMsgSize = GetUserMessage(i, app_wBuffer);

//		printf("Press Enter to Send \"%.40s...\" [Size %d]\n", app_wBuffer, nMsgSize);
//		if (mygetch() == '\r')	break;
//		fflush(stdin);

		if (nMsgSize <= 0) break;

		if ((nSent = send(hSock, app_wBuffer, nMsgSize, 0)) != -1 ) 
		{
			if (nSent != nMsgSize)
				printf("Only %d bytes from %d bytes are sent \n", nSent, nMsgSize);

			nLen = sizeof(servAddr);
			if ((nReceived = recv(hSock, app_wBuffer, APP_SND_BUF_SIZE, 0)) != -1 )
			{
				printf("[Echo %d] \"%.40s...\" \n", nReceived, app_wBuffer);
			}
			else
			{
				printf("[%04d] Socket Error\n", i); 
				break; 
			}
		}
		else
		{
			printf("[%04d] Socket Error\n", i); 
			break; 
		}
	}

//	getsockname(hSock, (struct sockaddr *)&servAddr, &nLen);
//	printf("My Address %s:%d\n", inet_ntoa(servAddr.sin_addr), ntohs(servAddr.sin_port));

/* 4 */
	printf("Press Enter to Close the Socket\n");
	mygetch();
	close(hSock);	

	return 0;
}


inline int GetUserMessage(int i, char * buf)
{
	int k;
	printf(">>");
	sprintf(buf, "%04d-", i);
	gets(buf+5);
	buf[k=strlen(buf)]=0;

	return k==5?0:k;
}


void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}


