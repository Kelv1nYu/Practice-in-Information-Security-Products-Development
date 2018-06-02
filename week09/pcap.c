#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<time.h>
#include<pcap.h>
#include<string.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<netinet/tcp.h>
#include<netinet/udp.h>
#include<net/ethernet.h>


void callback(u_char *useless, const struct pcap_pkthdr *pkthdr, const u_char *packet);

int main(int argc, char *argv[]){
    char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 mask;
    bpf_u_int32 net;
    struct in_addr addr;
    char *cnet;
    char *cmask;
    struct pcap_pkthdr header;
    const u_char *packet;
    pcap_t *handle;
    struct tm *now_tm;
    time_t now;
    struct bpf_program fp;
    char filter_exp[] = "ip";

    dev = pcap_lookupdev(errbuf);
    if(NULL == dev){
        fprintf(stderr, "DO NOT FIND DEFAULT DEVICE: %s\n", errbuf);
        return(2);
    }
    printf("Device:%s\n", dev);

    if(-1 == pcap_lookupnet(dev, &net, &mask, errbuf)){
        fprintf(stderr, "DO NOT GET NETMASK FOR DEVICE: %s\n", dev);
        cnet = 0;
        cmask = 0;
        return(2);
    }

    addr.s_addr = net;
    cnet = inet_ntoa(addr);
    printf("Net:%s\n", cnet);

    addr.s_addr = mask;
    cmask = inet_ntoa(addr);
    printf("Mask:%s\n", cmask);

    printf("=====================================================\n");

    handle = pcap_open_live(dev, PCAP_ERRBUF_SIZE, 1, 0, errbuf);
    if(NULL == handle){
        fprintf(stderr, "DO NOT OPEN DEVICE %s:%s\n", dev, errbuf);
        return(2);
    }

    if(-1 == pcap_compile(handle, &fp, filter_exp, 0, net)){
        fprintf(stderr, "DO NOT PARSE FILTER %s:%s\n", filter_exp, pcap_geterr(handle));
        return(2);
    }
	
	/*start on bpf*/
    if(pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return(2);
    }

	/*catch one packet*/
    packet = pcap_next(handle, &header);
    printf("Jacked one packet at %s\n", filter_exp);
	
	/*
	* Each packet in the dump file is prepended with this generic header.
	* This gets around the problem of different headers for different
	* packet interfaces.
	* struct pcap_pkthdr {
	* struct timeval ts;      * time stamp *
	* bpf_u_int32 caplen;     * length of portion present *
	* bpf_u_int32 len;        * length this packet (off wire) *
	* };				
	*/
	
	/*print the length of packet*/
    printf("Length of this packet is [%d] bytes\n", header.len);
    now = header.ts.tv_sec;
    now_tm = localtime(&now);
    printf("Time Stamp of this packet is [%d/%2d/%2d %2d:%2d:%2d]\n",
        now_tm->tm_year+1900, now_tm->tm_mon+1, now_tm->tm_mday,
        now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
	
	/*print the time of catching packet*/ 	
    printf("Recieved at ..... %s\n",ctime((const time_t*)&header.ts.tv_sec));
	
	/* call the callback()*/
    
    if (pcap_loop(handle, 4, callback, NULL) == -1){ 
        fprintf(stderr, "error\n");
        return(2);
    }
	
	/*close bpf*/
    pcap_close(handle);
    return(0);
}

void callback(u_char *useless, const struct pcap_pkthdr* pkthdr,const u_char* packet){
    int count = 0;    
    int otherPart_len;  
    int IPlength;      
    int n;
    static int num = 1;

    printf("Packet number:%d\n", num);
    
	
    struct ether_header *frame;   
    struct iphdr *IPpacket;       
    struct tcphdr *TCPpacket;    
    struct udphdr *UDPpacket;   
    struct in_addr srcIP, destIP;  

    

    frame = (struct ether_header *)packet;
	
    IPpacket = (struct iphdr *)(packet + sizeof(struct ether_header));
	
    TCPpacket = (struct tcphdr *)(packet + sizeof(struct ether_header) + sizeof(struct iphdr));
	
    UDPpacket = (struct udphdr *)(packet + sizeof(struct ether_header) + sizeof(struct iphdr));
	
    //print address
    printf("the DestMac is : %02X:%02X:%02X:%02X:%02X:%02X the SrcMac is : %02X:%02X:%02X:%02X:%02X:%02X\n", frame->ether_dhost[0],frame->ether_dhost[1],frame->ether_dhost[2],frame->ether_dhost[3], frame->ether_dhost[4],frame->ether_dhost[5],frame->ether_shost[0],frame->ether_shost[1], frame->ether_shost[2],frame->ether_shost[3],frame->ether_shost[4],frame->ether_shost[5]);
	
    //print address
    memcpy(&srcIP, &IPpacket->saddr, sizeof(IPpacket->saddr));
    memcpy(&destIP, &IPpacket->daddr, sizeof(IPpacket->daddr));
    IPlength = ntohs(IPpacket->tot_len);
    printf("the srcIP is : %s the destIP is : %s\n",inet_ntoa(srcIP),inet_ntoa(destIP));
	
    //print tcp info
    if(IPpacket->protocol == IPPROTO_TCP)
    {
		printf("the protocol of the packet is TCP:\n");
        printf("srcport: %d destport: %d\n", ntohs(TCPpacket->source), ntohs(TCPpacket->dest));
		
        otherPart_len = IPlength - sizeof(struct iphdr) - sizeof(struct tcphdr);
        const u_char *data = packet + sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct tcphdr);
		
		//print data
        while(otherPart_len > 0)
        {
            if(otherPart_len < 16)
            {
                printf("%5d    ",count);
                for(n=0; n < otherPart_len; n++)
                {
                    printf("%02X ", data[count+n]);
                    if((n+1) % 8 == 0)
                        printf(" ");
                }
                printf("   ");
                for(n=0; n < otherPart_len; n++)
                    if(isprint(data[count+n]))
                        printf("%c", data[count+n]);
                    else
                        printf(".",data[count+n]);
					printf("\n");
				otherPart_len = 0;
            }
            else
            {
                printf("%5d    ",count);
                for(n = 0; n < 16; n++)
                {
                    printf("%02X ",data[count+n]);
                    if((n+1) % 8 == 0)
                        printf(" ");
                }
                printf("   ");
                for(n = 0; n < 16; n++)
                {
                    if(isprint(data[count+n]))
                        printf("%c",data[count+n]);
                    else
                        printf(".",data[count+n]);
                }
                printf("\n");
                otherPart_len = otherPart_len - 16;
                count = count + 16;  
	    }
        } 
    }
	
    //print the udp info
    else if(IPpacket->protocol == IPPROTO_UDP)
    {
		printf("the protocol of the packet is UDP:\n");
        printf("srcport: %d destport: %d\n",ntohs(UDPpacket->source), ntohs(UDPpacket->dest));
		
        otherPart_len = IPlength - sizeof(struct iphdr) - sizeof(struct udphdr);
        const u_char *data = packet + sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct udphdr);
		
		//print the data
        while(otherPart_len > 0)
        {
            if(otherPart_len < 16)
            {
                printf("%5d    ",count);
                for(n = 0; n < otherPart_len; n++)
                {
                    printf("%02X ",data[count+n]);
                    if((n+1) % 8 == 0)
                        printf(" ");
                }
                printf("   ");
                for(n = 0; n < otherPart_len; n++)
                    if(isprint(data[count+n]))
                        printf("%c",data[count+n]);
                    else
                        printf(".", data[count+n]);
					printf("\n");
					otherPart_len = 0;
            }
            else
            {
                printf("%5d    ",count);
                for(n = 0; n < 16; n++)
                {
                    printf("%02X ",data[count+n]);
                    if((n+1) % 8 == 0)
                        printf(" ");
                }
                printf("   ");
                for(n = 0; n < 16; n++)
                {
                    if(isprint(data[count+n]))
                        printf("%c",data[count+n]);
                    else
                        printf(".",data[count+n]);
                }
                printf("\n");
                otherPart_len = otherPart_len - 16;
                count = count + 16;  
			}
        }
    } 
    else
    {
		printf("the protocol of the packet is others :\n");
    }
}
