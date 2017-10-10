#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>

int main(int argc, char **argv){
    pid_t pid;
    int f_des[2];
    char cmd[1024];

    printf("Please input your command:\n");
    scanf("%s",&cmd);

    if(-1 == pipe(f_des)){
        perror("create pipe error!\n");
        exit(1);
    }

    pid = fork();
    if(0 > pid){
        perror("create fork error!\n");
        exit(1);
    }else if(0 == pid){
        close(f_des[0]);
        dup2(f_des[1],STDOUT_FILENO);
        char *argv[] = {cmd,NULL};
        execvp(cmd,argv);
        exit(0);
    }else{
        close(f_des[1]);
        wait();
        char buffer[1024];
        int readByte = read(f_des[0], buffer, sizeof(buffer));
        buffer[readByte] = '\0';
        printf("The result is :\n%s\n",buffer);
        exit(0);
    }
    return 0;
}
