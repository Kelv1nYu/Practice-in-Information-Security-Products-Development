	#include <stdio.h>
	#include <arpa/inet.h>
  #include <pcap.h>
	
	#include <sys/time.h>
	#include <time.h>

	int main(int argc, char *argv[])
	{
		char *dev; //�豸��
		char  errbuf[PCAP_ERRBUF_SIZE];//���󻺳���
		bpf_u_int32 mask; // BPF��������������������ʽ����������
		bpf_u_int32 net; //BPF��ʽ�������ַ
		struct in_addr addr; 
		char *net_c; //�ַ�����ʽ�����ַ�����ڴ�ӡ���
		char *mask_c; //�ַ�����ʽ�����������ַ
		struct pcap_pkthdr header; //libpcap��ͷ�ṹ����������ʱ�䣬���񳤶������ݰ�ʵ�ʳ���
		const u_char *packet;//���񵽵�ʵ�����ݰ�����
		pcap_t *handle;//libpcap�豸��������

		struct tm *now_tm;
		time_t now;

		struct bpf_program fp;//������
	 	char filter_exp[] = "tcp port 80";//ʵ�ʵĹ��˹���

		/*��ȡ��������豸����ͨ��Ϊeth0*/
		dev = pcap_lookupdev(errbuf);
		if (dev == NULL) {
			fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
			return(2);
		}
		printf("Device: %s\n", dev);

		/*��ȡ�豸�������ַ�����������ַ���������ӡ����*/
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

		/*��ȡLibpcap bpf�����豸*/
		/*�豸��󲶻�����ֽ���Ϊ1518�������̫����*/
		/*��NIC��Ϊ����ģʽ��������������������������*/
		handle = pcap_open_live(dev, BUFSIZ, 1, 0, errbuf);
		if (handle == NULL) {
			fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
			return(2);
		}
		/*�����˹�����ӵ���������*/
		if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
			fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
			return(2);
		}
		/*��BPF���������������ݹ���*/
		if (pcap_setfilter(handle, &fp) == -1) {
		 	fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
		 	return(2);
	 	}
	 	/*����һ�����ݰ�*/
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
		/*��ӡ���ݰ��ĳ���*/
		printf("Length of this packet is [%d] bytes\n", header.len);
		
		now = header.ts.tv_sec;
		now_tm = localtime(&now);
		printf("Time Stamp of this packet is [%d/%2d/%2d %2d:%2d:%2d]\n",
			now_tm->tm_year+1900, now_tm->tm_mon+1, now_tm->tm_mday,
			now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
	  
	  /*��ӡ���ݰ��Ĳ���ʱ��*/ 	
		printf("Recieved at ..... %s\n",ctime((const time_t*)&header.ts.
tv_sec));
	
		/*�ر�bpf������*/
		pcap_close(handle);
		return(0);
	}
