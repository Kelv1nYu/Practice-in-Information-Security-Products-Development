#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<unistd.h>

#define PORT 8900
#define BUFFERSIZE 2048


struct arg{
    int connectd;
    struct sockaddr_in client;
};

void *pthread(void *arg);
int execute(char *command, char *buf);
int main(){
    int sockfd;
    int length;
    int port;
    int opt;
    pthread_t thread;
    struct arg *parg;

    struct sockaddr_in client;
    struct sockaddr_in server;

    port = PORT;

    //create sockfd
    opt = SO_REUSEADDR;
    if(-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0))){
        perror("create sockfd error\n");
        exit(1);
    }
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //bind
    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(-1 == bind(sockfd, (struct sockaddr*)&server, sizeof(struct sockaddr))){
        perror("bind socket error\n");
        close(sockfd);
        exit(1);
    }

    //listen
    if(-1 == listen(sockfd, 10)){
        perror("listen socket error\n");
        close(sockfd);
        exit(1);
    }

    length = sizeof(struct sockaddr_in);
    int connectd;
    while(1){
    //connect
        if(-1 == (connectd = accept(sockfd, (struct sockaddr*)&client, &length))){
            perror("connect socket error\n");
            close(connectd);
            exit(1);
            //continue;
        }

    //create thread
        parg = (struct arg*)malloc(sizeof(struct arg));
        parg->connectd = connectd;
        memcpy((void*)&parg->client, &client, sizeof(client));

        if(pthread_create(&thread, NULL, pthread, (void*)parg)){
            perror("create thread error\n");
            exit(1);
        }
    }
    close(sockfd);
}



void *pthread(void *arg){
    struct arg * info;
    info = (struct arg*)arg;

    char send_buf[BUFFERSIZE];
    char recv_buf[BUFFERSIZE];
    char cmd[BUFFERSIZE];
    int sendnum;
    int recvnum;
    int connectd;

    connectd = info->connectd;
    while(1){
        memset(send_buf, 0, BUFFERSIZE);
        memset(recv_buf, 0, BUFFERSIZE);
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

        printf("the server message is :%s\n", send_buf);

        if(0>send(connectd, send_buf, strlen(send_buf), 0)){
            perror("send error\n");
            //close(connectd);
            //break;
            continue;
        }
    }
    close(connectd);
    free(arg);
    pthread_exit(0);
}

int execute(char *command, char *buf){
    FILE *fp;
    int count;

    if(NULL == (fp = popen(command,"r"))){
        perror("create pipe error\n");
        exit(1);
    }

    count = 0;

    while(((buf[count] = fgetc(fp)) != EOF) && count < 2047){
        count++;
    }
    buf[count] = '\0';

    pclose(fp);
    return count;
}
