#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 8900
#define BUF_SIZE 2048

void print_usage(char *str){
	printf("%s usage:\n", str);
	printf("%s Ipaddr \n");
}

int main(int argc, char** argv){
	int sockfd;
	struct sockaddr_in client;
	struct sockaddr_in server;
	char send_buf[BUF_SIZE];
	char recv_buf[BUF_SIZE];
	int sendnum;
	int recvnum;
	int ret;
	int port;


	if(2!=argc)
	{
		print_usage(argv[0]);
		exit(1);	

	}
	
	memset(send_buf, 0, 2048);
	memset(recv_buf, 0, 2048);

	port = PORT;

	
	if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
	{
		perror("generating socket error\n");
		exit(1);

	}	

	memset(&server,0,sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(port);

	if(0 > (ret = connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr)))){
			perror("connect error");
			//close(sockfd);
			exit(1);
	}
 		
	
	while(1)
	{
		/*if(0 > (ret = connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr)))){
			perror("connect error");
			//close(sockfd);
			exit(1);
		}*/
		memset(send_buf, 0, 2048);
		memset(recv_buf, 0, 2048);
		
		printf("tcp>");
		gets(send_buf);
		printf("\n");
		
		if (0>send(sockfd, send_buf, strlen(send_buf), 0))
		{
			perror("error occar in sending data\n");
			close(sockfd);
			break;

		}
		if (0==strcmp(send_buf,"quit"))
		{
			perror("quitting remote ctroling\n");
			close(sockfd);
			break;
		}
		
		if (0>recv(sockfd, recv_buf, sizeof(recv_buf), 0))
		{
			perror("error occar in receiving data\n");
			close(sockfd);
			break;
		}
		
		fprintf(stdout,"%s\n",recv_buf);

	}
	close(sockfd);
	exit(1);
}
