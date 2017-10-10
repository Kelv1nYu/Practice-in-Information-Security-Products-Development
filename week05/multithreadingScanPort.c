#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>

#define THREADNUM 100
#define SCANLENGTH 655
#define END_PORT 65535

struct Port{
    int scan_minPort;
    int scan_maxPort;
    char *ip;
};

void *scanPort(void *arg);

int main(int argc, char **argv){
    int ret;
    struct sockaddr_in scanip;
    struct servent *servname;

    if(2 > argc || argc >6){
        perror("please input again\n")	;
        exit(1);
    }

    pthread_t *thread;
    thread = (pthread_t*)malloc(THREADNUM*sizeof(pthread_t));

    int i;
    int j;
    for(i = 1; i < argc; ++i){
        for(j = 0; j < THREADNUM; ++j){
            struct Port *port = (struct Port*)malloc(sizeof(struct Port));
            port->ip = argv[i];
            port->scan_minPort = j * SCANLENGTH + 1;

            if(i == (THREADNUM-1)){
                port->scan_maxPort = END_PORT;
            }
            else{
                port->scan_maxPort = port->scan_minPort + SCANLENGTH - 1;
            }

            if(pthread_create(&thread[j], NULL, (void *)scanPort, (void *)port)){
                perror("create thread error\n");
                free(thread);
                exit(-2);
            }
        }
    }
    return 0;
}

void *scanPort(void *arg){
    pthread_detach(pthread_self());
    int sockfd;
    int ret;
    int opt = SO_REUSEADDR;
    struct sockaddr_in server;
    struct servent *servername;


    struct Port *port = (struct Port*)arg;

    int startPort = port->scan_minPort;
    int endPort = port->scan_maxPort;
    int scanPort;
    for(scanPort = startPort; scanPort <= endPort; ++scanPort){
	
        if(-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0))){
            perror("create socket error\n");
            exit(1);
        }
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        memset(&server, 0, sizeof(struct sockaddr));
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr(port->ip);
        server.sin_port = htons(scanPort);

        if(-1 == (ret=connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr)))){
            //perror("connect error\n");
            close(sockfd);
            continue;
        }
        else{
            if(NULL == (servername = getservbyport(htons(scanPort),"tcp"))){
                printf("IP : %s, Port : %d, Unknown service\n", port->ip, scanPort);
            }
            else{
                printf("IP : %s, Port : %d, servername : %s\n",port->ip, scanPort, servername->s_name);
            }
        }
        close(sockfd);
    }
}
