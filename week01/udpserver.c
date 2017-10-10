#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>


#define PORT 8900
#define BUFFER_SIZE 1024

int main(int argc, char **argv){
	struct sockaddr_in server;
	struct sockaddr_in client;
	int sockfd;
	int ret;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd<0){
		perror("socket error");
	}

	memset(&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(PORT);

	//bind
	ret = bind(sockfd, (struct sockaddr *)&server, sizeof(server));
	if(ret<0){
		perror("bind error");
	}

	while(1){
		char buffer[BUFFER_SIZE];
		char send_buf[BUFFER_SIZE];
		struct sockaddr_in addressClient;
		socklen_t clientLen = sizeof(addressClient);
		memset(buffer, '\0', BUFFER_SIZE);

		if(recvfrom(sockfd, buffer, BUFFER_SIZE-1,0,(struct sockaddr*)&addressClient, &clientLen) == -1){
			perror("recive failed");
		}
		printf("Client:%s\n",buffer);
		
		char *normal_data = "I have recived the message!";
		if(sendto(sockfd, normal_data, strlen(normal_data),0,(struct sockaddr*)&addressClient,sizeof(addressClient)) < 0) { 
			perror("Send File Name Failed:");
			exit(1); 
		}

		if (0==strcmp(buffer,"quit")){
		perror("the Client break the server process\n");
		break;
		}
	}

	close(sockfd);
	return 0;

}
