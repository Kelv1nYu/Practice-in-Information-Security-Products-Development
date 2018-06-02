#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define CLIENTPORT 1573
#define BUFSIZE 2048

int main(int argc, char *argv[])
{
	int sockfd;
	fd_set sockset;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	int recvbytes;
	char recv_buf[BUFSIZE];
	char send_buf[BUFSIZE];
	int data_len;

	if (2 > argc)
	{
		printf("Please input the server ip!\n");
		exit(1);
	}
	if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0 )))
	{
		printf("create the socket error!\n");
		perror("create errors are :");
		exit(1);
	}
	memset(&serveraddr, 0, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port = htons(CLIENTPORT);

	if (-1 == connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr)))
	{
		perror("connect error:");
		exit(1);
	}
	
	memset(&send_buf,0,2048);
	fprintf(stderr,"welcome to visite the chat server\n");
	fprintf(stderr,"please input your name:");
	fgets(send_buf, 256, stdin);
	
	if (0 >= send(sockfd, send_buf, strlen(send_buf),0))
	{
		perror("sending data error\n");
		close(sockfd);
		exit(1);
	}
	
	
  FD_ZERO(&sockset); 
	FD_SET(sockfd, &sockset); 
	FD_SET(0, &sockset); 
	
	fprintf(stdout, "client ok!\n");
	fflush(stdout);


    while (1)
    {
		memset(recv_buf, 0, sizeof(recv_buf));
		memset(send_buf, 0, sizeof(send_buf));
		select(sockfd+1, &sockset, NULL, NULL, NULL); 
		if (FD_ISSET(sockfd, &sockset)) 
		{
			if (-1 == (recvbytes =read(sockfd, recv_buf, sizeof(recv_buf))))
			{
				perror("read data error:");
				close(sockfd);
				exit(1);
			}
			recv_buf[recvbytes] = '\0';
			printf("%s\n", recv_buf);
			fflush(stdout);
		}
		if (FD_ISSET(0, &sockset)) 
		{
			fgets(send_buf, sizeof(send_buf), stdin);
			data_len = strlen(send_buf);
			send_buf[data_len - 1] = '\0';
			//send the content
			if (-1 == write(sockfd, send_buf, sizeof(send_buf)))
			{
				perror("send data error:");
				close(sockfd);
				exit(1);
			} 
			//quit the chat room
                        char *cmd1 = "/quit";
			if (0 == strcmp(send_buf, cmd1))
			{
				printf("quiting from chat room!\n");
				close(sockfd);
				exit(1);
			}
		}
			FD_ZERO(&sockset);
			FD_SET(sockfd, &sockset);
			FD_SET(0, &sockset);
	}//end while
	close(sockfd);

	return 0;
}

