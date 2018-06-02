#include <stdio.h>
#include <libnet.h>

int main()
{
	int packet_size=0; //data package length
	libnet_t *l;       //libnet handle
	char *device="ens33"; //device name
	char error_information[LIBNET_ERRBUF_SIZE];
	
	//初始化，返回的是句柄
	l = libnet_init(LIBNET_LINK,device,error_information);
	if (NULL == l)
	{
		fprintf(stderr, "libnet_init(): %s",error_information);
	}
	
	char *destination_ip_str = "192.168.0.53";
	char *source_ip_str = "192.168.0.53";
	//u_char src_mac[6] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06}; /* 源MAC */  
    u_char dst_mac[6] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11}; /* 目的MAC */  
	// u_char payload[255] = {0}; /* payload:  承载数据的数组，初值为空 */  
	// u_long payload_len = 0; /* 承载数据的长度，初值为0 */  

	u_long  destination_ip;
	u_long  source_ip;
	u_int16_t src_port = 0;
	u_int16_t dst_port = 22;
	
	int result;

	// 把源、目标IP转成网络字节序                      
	destination_ip = libnet_name2addr4(l,destination_ip_str,LIBNET_RESOLVE);
	//source_ip = libnet_name2addr4(l,source_ip_str,LIBNET_RESOLVE);
	
	libnet_seed_prand(l); //get random seed 

	// strncpy(payload, "test", sizeof(payload)-1);  构造负载的内容   
    // payload_len = strlen(payload); /* 计算负载内容的长度 */ 

	int build_ip_flag = 1;
	libnet_ptag_t udp_tag = 0;
	libnet_ptag_t ip_tag = 0;
	libnet_ptag_t eth_tag = 0;	


	while(1){
		src_port = libnet_get_prand(LIBNET_PRu16);
		// 构造udp
		
		udp_tag = libnet_build_udp( src_port,
									dst_port,
									LIBNET_UDP_H, // payload大小为0，这里只加上头部
									0, //校验和设成0，由libnet自动计算
									NULL, // payload
									0, // payload_len
									l, // libnet句柄
									udp_tag // 表明是新建的数据包 
								  );
		if (-1 == udp_tag)
	  	{
	  		fprintf(stderr,"libnet_build_udp error: %s",libnet_geterror(l));	
	  	} 


	  	if(build_ip_flag){
	  		// 构造IP
		  	build_ip_flag = 0;
		  	ip_tag	= libnet_build_ipv4(LIBNET_IPV4_H + LIBNET_UDP_H, // IP协议块总长
		  								0, // tos
		  								(u_short) libnet_get_prand(LIBNET_PRu16), /* id,随机产生0~65535 */  
								        0, /* frag 片偏移 */  
								        (u_int8_t)libnet_get_prand(LIBNET_PR8), /* ttl,随机产生0~255 */  
		  								IPPROTO_UDP, /* 上层协议 */  
								        0, /* 校验和，此时为0，表示由Libnet自动计算 */  
								        source_ip, /* 源IP地址,网络序 */  
								        destination_ip, /* 目标IP地址,网络序 */  
								        NULL, /* 负载内容或为NULL */  
								        0, /* 负载内容的大小*/  
								        l, /* Libnet句柄 */  
								        0 /* 协议块标记可修改或创建,0表示构造一个新的*/ 
		  							);
		  	if (-1 == ip_tag)
		  	{
		  		fprintf(stderr,"libnet_build_ipv4 error: %s",libnet_geterror(l));	
		  	} 

		  	// 构造链路层的包
		  									
			eth_tag = libnet_build_ethernet((u_int8_t *)&dst_mac,
														(u_int8_t *)&dst_mac,
														ETHERTYPE_IP,
														NULL,
														0,
														l,
														0);			
														
			if (-1 == eth_tag)
			{
				 fprintf(stderr,"libnet_build_ethernet error: %s",libnet_geterror(l));	
			}
			  	
	  	}

  		


		result = libnet_write(l);
		if (-1 == result)
		{
			fprintf(stderr, "libnet_write error: %s\n", libnet_geterror(l));
		}else{
			printf("send successful\n");
		}				

		
		sleep(1.5);
	}

	
	
	libnet_destroy(l);
	return 0;
}

