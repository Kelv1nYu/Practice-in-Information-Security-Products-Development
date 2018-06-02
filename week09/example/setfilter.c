	#include <stdio.h>
	#include <arpa/inet.h>
  #include <pcap.h>
	
	#include <sys/time.h>
	#include <time.h>

	int main(int argc, char *argv[])
	{
		char *dev; //设备名
		char  errbuf[PCAP_ERRBUF_SIZE];//错误缓冲区
		bpf_u_int32 mask; // BPF（伯克利包过滤器）格式的网络掩码
		bpf_u_int32 net; //BPF格式的网络地址
		struct in_addr addr; 
		char *net_c; //字符串形式网络地址，用于打印输出
		char *mask_c; //字符串形式的网络掩码地址
		struct pcap_pkthdr header; //libpcap包头结构，包含捕获时间，捕获长度与数据包实际长度
		const u_char *packet;//捕获到的实际数据包内容
		pcap_t *handle;//libpcap设备描述符号

		struct tm *now_tm;
		time_t now;

		struct bpf_program fp;//过滤器
	 	char filter_exp[] = "tcp port 80";//实际的过滤规则

		/*获取活动的网络设备名，通常为eth0*/
		dev = pcap_lookupdev(errbuf);
		if (dev == NULL) {
			fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
			return(2);
		}
		printf("Device: %s\n", dev);

		/*获取设备的网络地址与网络掩码地址，并将其打印出来*/
		if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
			fprintf(stderr, "Can't get netmask for device %s\n", dev);
			net_c = 0;
			mask_c = 0;
			return(2);
	 	}
		addr.s_addr = net;
		net_c = inet_ntoa(addr);
		printf("Net: %s\n", net_c);

		addr.s_addr = mask;
  		mask_c = inet_ntoa(addr);
		printf("Mask: %s\n",mask_c);

		printf("==================================================\n");

		/*获取Libpcap bpf过滤设备*/
		/*设备最大捕获包的字节数为1518（针对以太网）*/
		/*将NIC设为混在模式，可以侦听局域网的所有内容*/
		handle = pcap_open_live(dev, BUFSIZ, 1, 0, errbuf);
		if (handle == NULL) {
			fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
			return(2);
		}
		/*将过滤规则添加到过滤器上*/
		if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
			fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
			return(2);
		}
		/*在BPF过滤器上启动数据过滤*/
		if (pcap_setfilter(handle, &fp) == -1) {
		 	fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
		 	return(2);
	 	}
	 	/*捕获一个数据包*/
		packet = pcap_next(handle, &header);
		
		printf("Jacked one packet at %s\n", filter_exp);
		/*
		 * Each packet in the dump file is prepended with this generic header.
		 * This gets around the problem of different headers for different
		 * packet interfaces.
		 * struct pcap_pkthdr {
	 	 *         struct timeval ts;      * time stamp *
		 *         bpf_u_int32 caplen;     * length of portion present *
		 *         bpf_u_int32 len;        * length this packet (off wire) *
		 * };
		 */
		/*打印数据包的长度*/
		printf("Length of this packet is [%d] bytes\n", header.len);
		
		now = header.ts.tv_sec;
		now_tm = localtime(&now);
		printf("Time Stamp of this packet is [%d/%2d/%2d %2d:%2d:%2d]\n",
			now_tm->tm_year+1900, now_tm->tm_mon+1, now_tm->tm_mday,
			now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
	  
	  /*打印数据包的捕获时间*/ 	
		printf("Recieved at ..... %s\n",ctime((const time_t*)&header.ts.
tv_sec));
	
		/*关闭bpf过滤器*/
		pcap_close(handle);
		return(0);
	}
