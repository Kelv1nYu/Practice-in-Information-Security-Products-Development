#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<unistd.h>

#define PORT 8900
#define BUF_SIZE 2048

int execute(char *command, char *buf);

int main(){
	int sockfd;
	struct sockaddr_in client;
	struct sockaddr_in server;
	char send_buf[BUF_SIZE];
	char recv_buf[BUF_SIZE];
	char cmd[2048];
	int sendnum;
	int recvnum;
	int len;
	int opt;
	int connectd;
	int port;

	

	port = PORT;

	if(-1==(sockfd = socket(AF_INET, SOCK_STREAM, 0))){
		perror("generating socket error\n");
		exit(1);
	}

	memset(&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	opt = SO_REUSEADDR;

	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	if(-1 == bind(sockfd,(struct sockaddr*)&server, sizeof(struct sockaddr))){
		perror("bind error\n");
		exit(1);
	}

	if(-1 == listen(sockfd,5)){
		perror("listen error\n");
		exit(1);
	}

	/*if(-1 == (connectd = accept(sockfd,(struct sockaddr*)&client,&len))){
			perror("create connect socket error\n");
			exit(1);
		}*/

	while(1){

		if(-1 == (connectd = accept(sockfd,(struct sockaddr*)&client,&len))){
			perror("create connect socket error\n");
			//close(sockfd);
			//exit(1);
			continue;
		}

		pid_t pid;
		pid = fork();
		if(0 > pid){
			perror("create fork error!\n");
			exit(1);
		}else if(0 < pid){
			continue;
		}else{

			while(1){
				memset(send_buf, 0, 2048);
				memset(recv_buf, 0, 2048);

				if(0 > (recvnum = recv(connectd, recv_buf, sizeof(recv_buf), 0))){
					perror("recv error\n");
					close(connectd);
					continue;
				}
				recv_buf[recvnum]='\0';

				if(0 == strcmp(recv_buf,"quit")){
					perror("quitting remote controling\n");
					close(connectd);
					break;
				}

				printf("the message is : %s\n", recv_buf);

				strcpy(cmd, "/bin/");
				strcat(cmd, recv_buf);
				execute(cmd, send_buf);

				if('\0' == *send_buf){
					memset(cmd, 0, sizeof(cmd));
					strcpy(cmd, "/sbin/");
					strcat(cmd, recv_buf);
					execute(cmd, send_buf);

					if('\0' == *send_buf){
						memset(cmd, 0, sizeof(cmd));
						strcpy(cmd, "/usr/bin/");
						strcat(cmd, recv_buf);
						execute(cmd, send_buf);
					}
					if('\0' == *send_buf){
						memset(cmd, 0, sizeof(cmd));
						strcpy(cmd, "/usr/sbin/");
						strcat(cmd, recv_buf);
						execute(cmd, send_buf);
					}
				}
				if('\0' == *send_buf){
					sprintf(send_buf, "command is not vaild, check it please\n");
				}

				printf("the server message is :\n%s\n", send_buf);

				if(0>send(connectd, send_buf, strlen(send_buf), 0)){
					perror("send error\n");
					close(connectd);
					break;
				}
			}		
		}
	}
	close(sockfd);
	exit(1);
}

int execute(char *command, char *buf){
	FILE *fp;
	int count;

	if(NULL==(fp = popen(command, "r"))){
		perror("creating pipe error\n");
		exit(1);
	}

	count = 0;

	while(((buf[count] = fgetc(fp)) != EOF ) && count < 2047){
		count++;
	}
	buf[count] = '\0';

	pclose(fp);

	return count;

}
		
