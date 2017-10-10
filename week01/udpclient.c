#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>

#define PORT 8900
#define BUFFER_SIZE 1024

int main(int argc, char **argv){
	int fd;
	int check_ret;
	char send_buf[BUFFER_SIZE];

	fd = socket(PF_INET,SOCK_DGRAM , 0);
	assert(fd >= 0);

	struct sockaddr_in address;
	memset(&address, 0, sizeof(struct sockaddr_in));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(PORT);
	//send message
	//const char* normal_data = "my boy!";

	gets(send_buf);
	if(sendto(fd, send_buf, strlen(send_buf),0,(struct sockaddr*)&address,sizeof(address)) < 0) { 
		perror("Send File Name Failed:"); 
		exit(1); 
	}
	
	//recive the message
	char buffer[BUFFER_SIZE];
        struct sockaddr_in addressServer;
        socklen_t serverLen = sizeof(addressServer);
        memset(buffer, '\0', BUFFER_SIZE);

        if(recvfrom(fd, buffer, BUFFER_SIZE-1,0,(struct sockaddr*)&addressServer, &serverLen) == -1) 
        { 
           perror("Receive Data Failed:"); 
           exit(1); 
        } 
        printf("Server:%s\n", buffer);

	close(fd);
}
