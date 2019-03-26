#include<stdio.h>
#include<pcap.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define u_char u_int8_t
#define u_short u_int16_t
#define u_int u_int32_t

#define ETHER_ADDR_LEN 6

struct ether_header{
	u_char ether_dhost[ETHER_ADDR_LEN];
	u_char ether_shost[ETHER_ADDR_LEN];
	u_short ether_type;
	/* 0x0800: printf("上层协议是IP协议
	0x0806: printf("上层协议是ARP协议\n")
	0x8035: printf("上层协议是RARP协议\n")*/
};


#define IP_RF 0x8000
#define IP_DF 0x4000
#define IP_MF 0x2000
#define IP_OFFMASK 0x1fff

#define IP_HL(ip) (((ip)->ip_vhl)& 0x0f) /*ip header length by 4 bytes*/
#define IP_V(ip)  (((ip)->ip_vhl)>>4)  /*ip version general is 4*/

struct ip_header{
	u_char ip_vhl;
	u_char ip_tos;
	u_short ip_len;
	u_short ip_id;
	u_short ip_off;

	u_char ip_ttl;
	u_char ip_p;
	/*case 1: printf("上层协议是ICMP协议\n"); 
	case 2: printf("上层协议是IGMP协议\n"); 
	case 6: printf("上层协议是TCP协议\n"); 
	case 17: printf("上层协议是UDP协议\n");*/

	u_short ip_sum;  /*checksum*/
	struct in_addr ip_src,ip_dest;
};

struct tcp_header{
	u_short th_sport;
	u_short th_dport;
	u_int th_seq;
	u_int th_ack;
	u_char th_offx2;

	u_char th_flags;

	u_short th_win;
	u_short th_sum;
	u_short th_urp;
};

#define ETHER_HEADER_LEN 14

void cook_packet(u_char* args,const struct pcap_pkthdr *header,const u_char* packet){
	struct ether_header *ether;
	struct ip_header *ip;
	struct tcp_header *tcp;
	char *payload;
    
	FILE* fp = *(FILE**)args;

	u_int ip_header_l;
	u_int tcp_header_l;
	
	u_char *mac_addr_slice;
	ether = (struct ether_header*)packet;
	/****************up wall******************/
	mac_addr_slice = ether->ether_dhost;
	fprintf(fp,"Ether Destination host:%2x:%2x:%2x:%2x:%2x:%2x\n",*mac_addr_slice,*(mac_addr_slice+1),*(mac_addr_slice+2),*(mac_addr_slice+3),*(mac_addr_slice+4),*(mac_addr_slice+5));
	mac_addr_slice = ether->ether_shost;
	fprintf(fp,"Ether Source host:%2x:%2x:%2x:%2x:%2x:%2x\n",*mac_addr_slice,*(mac_addr_slice+1),*(mac_addr_slice+2),*(mac_addr_slice+3),*(mac_addr_slice+4),*(mac_addr_slice+5));
/*******************down wall*****************/
	if(ntohs(ether->ether_type) != 0x0800){
		fprintf(fp,"Not IP proto\n\n\n");
		return;
	}
		
	ip=(struct ip_header*)(packet+ETHER_HEADER_LEN);
	ip_header_l = IP_HL(ip)*4;
	if(ip_header_l<20){
		fprintf(fp,"Invalid IP header length:%u bytes\n\n\n",ip_header_l);
		return;
	}
	/*****************up wall********************/
	fprintf(fp,"ip destination host:%s\n",inet_ntoa(ip->ip_dest));
	fprintf(fp,"ip source host:%s\n",inet_ntoa(ip->ip_src));
	/*****************down wall********************/
	if(ip->ip_p!=6 && ip->ip_p!=17){
		fprintf(fp,"Not TCP or UDP proto\n\n\n");
		return;
	}
	tcp=(struct tcp_header*)(packet+ETHER_HEADER_LEN+ip_header_l);
	/********************up wall**********************/
	fprintf(fp,"tcp/udp destination port:%hu\n",ntohs(tcp->th_dport));
	fprintf(fp,"tcp/udp source port:%hu\n\n\n",ntohs(tcp->th_sport));
	/*********************down wall*******************/
}

int main(){
	char *dev,errbuf[PCAP_ERRBUF_SIZE];
	int flag=-1;
	pcap_if_t *alldevs=NULL;
	pcap_if *next=NULL;
	flag = pcap_findalldevs(&alldevs,errbuf);
	
	char filename[]="capture.txt";
	FILE *fp = fopen(filename , "a+");
   	
	if(flag==-1){
		fprintf(stderr,"Couldn't find default device: %s\n",errbuf);
		return 2;
	}
	next=alldevs;
	/*while(next!=NULL){
		printf("name:%s\n",next->name);
		printf("description:%s\n\n",next->description);
		next=next->next;
	}
	printf("That's all devices\n");*/
	pcap_t *handle = pcap_open_live(next->name,BUFSIZ,1,1000,errbuf);
	if(handle==NULL){
		fprintf(stderr,"Couldn't open device %s:%s\n",next->name,errbuf);
		return 2;
	}
	
	if(pcap_datalink(handle)!=DLT_EN10MB){
		fprintf(stderr,"Device %s doesn't provide Ethernet headers\n",next->name);
		return 2;
	}
	pcap_loop(handle,10,cook_packet,(u_char *)&fp);

	fclose(fp);
	return 0;
}
