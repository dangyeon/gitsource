/* sample Codes for Network Programming Class (2013)
 *
 * Chaeseok, Lee (quarry@mobile.re.kr)
 * 
 * KyungSung University 
 *  
 */

// DatagramClient.c : 
// A Simple UDP Client Sending some messages to server

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define	APP_SND_BUF_SIZE	100

void ErrorHandling(char *message);
int GetUserMessage(int i, char * buf);

int main(int argc, char* argv[])
{
	int sock;		

	struct sockaddr_in servAddr;
	
	char app_wBuffer[APP_SND_BUF_SIZE+1];
	
/* 0 */
	if(argc!=3)
	{
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

/* 1 */ 	
	if ( (sock=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
		ErrorHandling("socket() error");

/* 2 */ 
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=inet_addr(argv[1]);
	servAddr.sin_port=htons(atoi(argv[2]));

/* 3 */ 
	int nMsgSize, nSent, i;
	for (i=1; i <= 10; i++)
	{
		nMsgSize = GetUserMessage(i, app_wBuffer);

		if ((nSent = sendto(sock, app_wBuffer, nMsgSize, 0,(struct sockaddr*)&servAddr, sizeof(servAddr))) != -1 ) 
		{
			if (nSent != nMsgSize)
				printf("Only %d bytes from %d bytes are sent \n", nSent, nMsgSize);
		}
		else
		{
			printf("[%04d] Socket Error \n", i); 
			break; 
		}
	}

/* 4 */
	close(sock);	
	return 0;
}


int GetUserMessage(int i, char * buf)
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



