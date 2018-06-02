#include <stdio.h>
#include <pcap.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(void)
{
	char * device_name;
	char * netaddr;
	char * netmask;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct in_addr addr;
	
	bpf_u_int32 netp;
	bpf_u_int32 maskp;

	device_name = NULL;
	netaddr = NULL;
	netmask = NULL;
	memset(errbuf,0,PCAP_ERRBUF_SIZE);

	netp = 0;
	maskp = 0;
	
	device_name = pcap_lookupdev(errbuf);
	if (NULL == device_name)
	{
		perror("can not get device name\n");
		perror("errorbuf\n");
		return 1;

	}

	fprintf(stderr,"the device name is %s\n",device_name);

	if (-1 == pcap_lookupnet(device_name,&netp,&maskp,errbuf))
	{
		perror("can not get the interface message\n");
		perror("errbuf\n");
		return 1;

	}

	addr.s_addr = netp;
	netaddr = inet_ntoa(addr);

	if (NULL == netaddr)
	{
		perror("can not convert ip addr\n");
		return 1;
	}

	fprintf(stderr,"the device net addr is %s\n",netaddr);
	

	addr.s_addr = maskp;
	netmask = inet_ntoa(addr);

	if (NULL == netmask)
	{
		perror("can not convert ip addr\n");
		return 1;
	}

	fprintf(stderr,"the device netmask is %s\n",netmask);
	
	return 0;

}
