#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 8900
#define BUF_SIZE 2048

int execute(char* command,char* buf);


int main()
{
	int sockfd;
	struct sockaddr_in client;
	struct sockaddr_in server;
	char send_buf[BUF_SIZE];
	char recv_buf[BUF_SIZE];
	char cmd[2048];
	int sendnum;
	int recvnum;
	int length;
	int port;
	
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
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	if (-1==bind(sockfd,(struct sockaddr*)&server,sizeof(struct sockaddr)))
	{
		perror("binding error\n");
		close(sockfd);
		exit(1);

	}
	
	length = sizeof(struct sockaddr_in);

	while(1)
	{
		memset(recv_buf,0,2048);
		memset(send_buf,0,2048);

		if(0>(recvnum = recvfrom(sockfd,recv_buf,sizeof(recv_buf),0,(struct sockaddr*)&client,&length)))
		{
			perror("error in recving data...\n");
			break;

		}	
		recv_buf[recvnum]='\0';

		if (0==strcmp(recv_buf,"quit"))
		{
			perror("quitting remote controling\n");
			break;

		}
		printf("the message is: %s\n",recv_buf);

		strcpy(cmd,"/bin/");
		strcat(cmd,recv_buf);
		execute(cmd,send_buf);

		if ('\0'==*send_buf)
		{
			memset(cmd,0,sizeof(cmd));
			strcpy(cmd,"/sbin/");
			strcat(cmd,recv_buf);
			execute(cmd,send_buf);
			
			if ('\0'==*send_buf)
			{	
				memset(cmd,0,sizeof(cmd));
				strcpy(cmd,"/usr/bin/");
				strcat(cmd,recv_buf);
				execute(cmd,send_buf);
			}
			
			if ('\0'==*send_buf)
			{	
				memset(cmd,0,sizeof(cmd));
				strcpy(cmd,"/usr/sbin/");
				strcat(cmd,recv_buf);
				execute(cmd,send_buf);
			}
		}
		if ('\0'==*send_buf)
			sprintf(send_buf,"command is not vaild,check it please\n");

		printf("the server message is:%s\n",send_buf);

		if (0>sendto(sockfd,send_buf,sizeof(send_buf),0,(struct sockaddr*)&client,length))
		{
			perror("sending data error\n");
			break;
		}

	}
	close(sockfd);
	exit(1);
}

int execute(char* command,char* buf)
{
	FILE * 	fp;
	int count;

	if (NULL==(fp = popen(command,"r")))
	{
		perror("creating pipe error\n");
		exit(1);

	}
	
	count = 0 ;

	while(((buf[count] = fgetc(fp))!=EOF)&&count<2047)
	count++;
	buf[count]='\0';

	pclose(fp);
	return count;
	

}

