
/*
 * Sample Codes for Network Programming Class (2013)
 *
 * Chaeseok, Lee (quarry@mobile.re.kr)
 * 
 * KyungSung University 
 *  
 */

// StreamClient_PressEnter.c: 
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define	APP_SND_BUF_SIZE	2000
#define SYS_SND_BUF_SIZE	100

void ErrorHandling(char *message);
inline int GetUserMessage(int i, char * buf);
int mygetch(void);

int main(int argc, char* argv[])
{
	int		hSock;		
	struct sockaddr_in servAddr;	
	char	app_wBuffer[APP_SND_BUF_SIZE+1];

/* 1 */
	if(argc!=3)
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

/* 2 */ 	
	if ( (hSock=socket(AF_INET, SOCK_STREAM, 0)) == -1)
		ErrorHandling("socket() error");

/* 3 */ 
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family			=AF_INET;
	servAddr.sin_addr.s_addr	=inet_addr(argv[1]);
	servAddr.sin_port			=htons(atoi(argv[2]));

/* 3-1 */ 
	if( connect(hSock, (struct sockaddr*)&servAddr, sizeof(servAddr))== -1 ) 
		ErrorHandling("connect() error!");

/* 4 */ 
	int nBufSize = SYS_SND_BUF_SIZE, nLen=sizeof(int);
	setsockopt(hSock, SOL_SOCKET, SO_SNDBUF, (char *) &nBufSize, sizeof(nBufSize));
	getsockopt(hSock, SOL_SOCKET, SO_SNDBUF, (char *) &nBufSize, &nLen);
	printf("[APP/SYS] Sending Buffer Size is Set to [%d/%d] Bytes\n", APP_SND_BUF_SIZE, nBufSize);


/* 4-a */ 
//	int opt_val = TRUE;
//	setsockopt(hSock, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt_val, sizeof(opt_val));


/* 5 */ 
	int nMsgSize, nSent, i;
	for (i=1; i < 10000; i++)
	{
		nMsgSize = GetUserMessage(i, app_wBuffer);

		printf("Press Enter to Send \"%.40s...\" [Size %d]\n", app_wBuffer, nMsgSize);
		if (mygetch() == '\r')	break;
		fflush(stdin);

		if ((nSent = send(hSock, app_wBuffer, nMsgSize, 0)) != -1) 
		{
			if (nSent != nMsgSize)
				printf("Only %d bytes from %d bytes are sent \n", nSent, nMsgSize);
		}
		else
		{
			printf("[%04d] Socket Error - Code\n", i); 
			break; 
		}
	}


/* 6 */
	printf("Press Enter to Close the Socket\n");
	mygetch();
	close(hSock);	

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


inline int GetUserMessage(int i, char * buf)
{
	int k;
	sprintf(buf, "%04d-", i);
	for (k = 5; k < APP_SND_BUF_SIZE; k++)
		buf[k] = '0' + (char)((k+1)%10);
	buf[k]=0;

	return k;
}


void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

