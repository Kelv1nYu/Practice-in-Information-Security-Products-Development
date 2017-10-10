#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>

void print_usage(char * cmd)
{
	fprintf(stderr," %s usage:\n",cmd);
	fprintf(stderr,"%s IP_Addr [port]\n",cmd);

}

int scanPort(int sockfd, struct sockaddr_in scanip, int port, char** argv){
	int ret;

	if(-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0))){
		perror("create sockfd error\n");
		exit(1);
	}

	memset(&scanip, 0, sizeof(struct sockaddr));
	scanip.sin_family = AF_INET;
	scanip.sin_addr.s_addr = inet_addr(argv[1]);
	scanip.sin_port = htons(port);

	if(-1 == (ret=connect(sockfd, (struct sockaddr *)&scanip, sizeof(struct sockaddr)))){
			//perror("connect error\n");
			close(sockfd);
	}
	return ret;
}

int main(int argc, char **argv){
	int ret;
	int start_port = atoi(argv[2]);
	int end_port = atoi(argv[3]);
	struct sockaddr_in scanip;
	struct servent *servname;

	if(4 != argc){
		perror("please input again\n");
		exit(1);
	}

	/*memset(&scanip, 0, sizeof(struct sockaddr));
	scanip.sin_family = AF_INET;
	scanip.sin_addr.s_addr = inet_addr(argv[1]);*/
		
	int port = start_port;
	for(port; port <= end_port; port++){
	
		int sockfd;
		if(!scanPort(sockfd, scanip, port, argv)){

			if (-1==(sockfd=socket(AF_INET, SOCK_STREAM, 0))){
				perror("error in generate socket\n");
				exit(1);
			}
			connect(sockfd,(struct  sockaddr*)&scanip,sizeof(struct sockaddr)); 
			printf("%d   ", port);
			
			if(NULL == (servname = getservbyport(htons(port),"tcp"))){
				printf("unknown\n");
			}else{
				printf("%s\n",servname->s_name);
			}
		}
		
		close(sockfd);
		//port++;
	}

	return 0;
}
