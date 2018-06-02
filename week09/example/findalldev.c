#include <stdio.h>
#include <pcap.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main()
{
	char * device_name;
	char * netaddr;
	char * maskaddr;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct in_addr addr;
	//struct pcap_if pif;
	struct pcap_if* ppif;

	

	bpf_u_int32 netp;
	bpf_u_int32 maskp;

	device_name = NULL;
	netaddr = NULL;
	maskaddr = NULL;
	memset(errbuf,0,PCAP_ERRBUF_SIZE);
	ppif = NULL;
	//ppif = pif;
	
	
	if (-1 == pcap_findalldevs(&ppif,errbuf))
	{
		perror("find device error\n ");
		perror("errbuf\n");
		return 1;
	}
	
		
	if (NULL == ppif )
	{
		perror("can not get device message\n");
		return 0;
	}

	for (ppif;ppif!=NULL;ppif=ppif->next)
	{
		static count =1;
		if (NULL==ppif->name)continue;

		fprintf(stderr,"the %d device name is %s\n",count,ppif->name);
		count++;
		
		if (-1 == pcap_lookupnet(ppif->name,&netp,&maskp,errbuf))
		{
			perror("can not get the device message\n");
			continue;	
		}
		
		addr.s_addr = netp;
		netaddr = inet_ntoa(addr);

		if (NULL == netaddr)
		{
			perror("can not convert ip addr\n");
			continue;
		}

		fprintf(stderr,"the device net addr is %s\n",netaddr);

		addr.s_addr = maskp;
		maskaddr = inet_ntoa(addr);

		if (NULL == maskaddr)
		{
			perror("can not convert maks addr\n");
			continue;
		}

		fprintf(stderr,"the device net mask addr is %s\n",maskaddr);


	}

	return 0;
}
