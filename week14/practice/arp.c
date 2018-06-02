#include <stdio.h>
#include <libnet.h>

int main()
{
	libnet_t *l;       //libnet handle
	char *device=NULL; //device name
	char error_information[LIBNET_ERRBUF_SIZE];
	
	l = libnet_init(LIBNET_LINK,device,error_information);
	if (NULL == l)
	{
		fprintf(stderr, "libnet_init(): %s",error_information);
	}

	char *destination_ip_str = "192.168.0.53";
	char *source_ip_str = "192.168.0.53";
	//00:50:56:fb:3e:18
	u_int8_t srcmac[6] = {0x00,0x50,0x56,0xfb,0x3e,0x18};
	u_int8_t dstmac[6] = {0x11,0x11,0x11,0x11,0x11,0x11};

	u_long  destination_ip;
	u_long  source_ip;
	
	int result;

	destination_ip = libnet_name2addr4(l,destination_ip_str,LIBNET_RESOLVE);
	source_ip = libnet_name2addr4(l,source_ip_str,LIBNET_RESOLVE);
	
	
	libnet_ptag_t t1;
	t1 = libnet_build_arp(ARPHRD_ETHER,
				ETHERTYPE_IP,
				6,
				4,
				ARPOP_REPLY,
				srcmac,
				(u_int8_t*) &source_ip,
				dstmac,
				(u_int8_t*) &destination_ip,
				NULL,
				0,
				l,
				0);
	if (-1 == t1){
		fprintf(stderr,"libnet_build_arp error: %s",libnet_geterror(l));	
	} 


	libnet_ptag_t t2;
	t2 = libnet_build_ethernet(dstmac,
					srcmac,
					ETHERTYPE_ARP,
					NULL,
					0,
					l,
					0);
	
	if (-1 == t2){
		fprintf(stderr,"libnet_build_ethernet error: %s",libnet_geterror(l));	
	} 
			

	while(1){
		result = libnet_write(l);
		if (-1 == result){
			fprintf(stderr, "libnet_write: %s\n", libnet_geterror(l));
		}
		printf("arp packet has been sent\n");
		sleep(1);										 
	}
	
	libnet_destroy(l);
	return 0;
}
