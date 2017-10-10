#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 8900
#define BUF_SIZE 2048

void print_usage(char* str)
{
	printf("%s usage :\n",str);
	printf("%s Ipaddr \n");

}


int main(int argc, char** argv)
{
	int sockfd;
	struct sockaddr_in client;
	struct sockaddr_in server;
	char send_buf[BUF_SIZE];
	char recv_buf[BUF_SIZE];
	int sendnum;
	int recvnum;
	int length;
	int port;


	if(2!=argc)
	{
		print_usage(argv[0]);
		exit(1);	

	}
	
	memset(send_buf,0,2048);
	memset(recv_buf,0,2048);

	port = PORT;

	
	if (-1==(sockfd=socket(AF_INET,SOCK_DGRAM,0)))
	{
		perror("generating socket error\n");
		exit(1);

	}	

	memset(&server,0,sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(port);

	
 	length = sizeof(struct sockaddr);	
	
	while(1)
	{
		printf("udp>");
		gets(send_buf);
		printf("\n");
		
		if (0>sendto(sockfd,send_buf,strlen(send_buf),0,(struct sockaddr*)&server,length))
		{
			perror("error occar in sending data\n");
			break;

		}
		if (0==strcmp(send_buf,"quit"))
		{
			perror("quitting remote ctroling\n");
			break;
		}
		
		if (0>recvfrom(sockfd,recv_buf,sizeof(recv_buf),0,(struct sockaddr*)&client,&length))
		{
			perror("error occar in receiving data\n");
			break;
		}
		
		fprintf(stdout,"%s\n",recv_buf);

	}
	close(sockfd);
	exit(1);
}



