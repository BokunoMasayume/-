#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<pthread.h>

#include<string.h>
#include<sys/types.h>
#include<ifaddrs.h>

#include<mysql/mysql.h>

#include<openssl/md5.h>
#include<openssl/aes.h>

#include<time.h>

#define MAXBUF 1024
//g++ server.cpp -o server -lpthread -I/usr/include -L/usr/lib/mysql -lmysqlclient
MYSQL mysql;
void *tranfer_file_2clnt(void* clnt_sock);
char* get_host_ip(char* addr);

int main(){
	
	mysql_init(&mysql);
	mysql_real_connect(&mysql , "localhost","root","mushroom","lab4",0,NULL,0);
	int serv_sock = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);

	struct sockaddr_in serv_addr;

	char host_addr_ip[INET_ADDRSTRLEN];
	if(!get_host_ip(host_addr_ip)){
		printf("Fail to get local host ip");
		return 0;
	}	
	else{
		printf("server will bind to %s :%d\n",host_addr_ip,8777);
	}

	memset(&serv_addr ,0 ,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(host_addr_ip);
	serv_addr.sin_port = htons(8777);

	bind(serv_sock , (struct sockaddr*)&serv_addr , sizeof(serv_addr));
    //func bind passed the int 'size'
	listen(serv_sock,20);

	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size = sizeof(clnt_addr);

	while(1){

         
		int clnt_sock = accept(serv_sock , (struct sockaddr*)&clnt_addr , &clnt_addr_size);
		//func accept passed szie's point
                printf("accepted\n");

		pthread_t thrd;
		pthread_create(&thrd , NULL,tranfer_file_2clnt ,(void*)&clnt_sock);
   
    		
	}
	close(serv_sock);

	return 0;
}

void *tranfer_file_2clnt(void *clnt_sock){
	int sock = *(int *)clnt_sock;
	int numRow=0;
	char res_str[] = "bao ta zhen he yao";
	char rec_str[] = "tian wang gai di hu";
	char buffer[MAXBUF]={0};
	char cmd[300]="\0";
	char hash1[33]="\0";
	char hash2[33]="\0";
	char name[150];
	char in_name[150];
	char in_token[10];
	char in_hash2[33];
	char tmp[3];
	char out_token_str[17];
	char test_str[17]={'\0'};
	char encode_out_token[17];
	int out_token;
	int n=0;
	unsigned char md[16];
	MD5_CTX ctx;
	MYSQL_RES *sql_res = NULL;
	MYSQL_ROW row;
	n = read(sock ,buffer ,MAXBUF);
	buffer[n] = '\0';
	if(strcmp(buffer,rec_str)){
		printf("not valid\n");
		write(sock , "error",5);
		close(sock);
		pthread_exit(NULL);
	}
	write(sock , res_str , strlen(res_str));
	n = read(sock,buffer,MAXBUF);
	buffer[n] = '\0';
	if(strcmp(buffer,"signup")==0){
		write(sock , "ok",2);
		n = read(sock , buffer,MAXBUF);//read hash2 name token
		buffer[n] = '\0';
		sscanf(buffer,"%s %s %s",in_name,in_token,in_hash2);
		snprintf(cmd , sizeof(cmd),"select * from user where name='%s'",in_name);
		if(mysql_query(&mysql , cmd)){
			write(sock,"error",5);
			close(sock);
			pthread_exit(NULL);
		}
		sql_res = mysql_store_result(&mysql);
		numRow = mysql_num_rows(sql_res);
		if(numRow==0){
			write(sock,"empty" , 5);
			close(sock);
			pthread_exit(NULL);
		}
		row = mysql_fetch_row(sql_res);
		snprintf(hash1,sizeof(hash1) , "%s",row[1]);
		printf("hash1:%s\n",hash1);
		MD5_Init(&ctx);
		MD5_Update(&ctx , hash1,strlen(hash1));
		MD5_Update(&ctx , in_token , strlen(in_token));
		MD5_Final(md , &ctx);
		for(int i=0;i<16;i++){
			snprintf(tmp,sizeof(tmp),"%02x",md[i]);
			strcat(hash2,tmp);
		}
		if(strcmp(hash2,in_hash2)){
			write(sock,"error",5);
			close(sock);
			pthread_exit(NULL);
		}
		printf("right my user\nhash1: ");
		for(int i=0;i<16;i++){
			sscanf((hash1+i*2),"%2hhx",&md[i]);
			printf("%2hhx ",md[i]);
		}
		printf("\n");
		srand((unsigned)time(NULL));
		out_token = rand();
		snprintf(out_token_str,sizeof(out_token_str) , "%16d" , out_token);
		printf("out token :%s\n",out_token_str);
		AES_KEY aes;
		AES_set_encrypt_key(md,128,&aes);
		AES_ecb_encrypt((unsigned char*)out_token_str,(unsigned char*)encode_out_token,&aes , AES_ENCRYPT);
		printf("encode token :%s\n",encode_out_token);
		hash2[0]='\0';
		for(int i=0;i<16;i++){
			snprintf(tmp,sizeof(tmp),"%02x",(unsigned char)encode_out_token[i]);
			strcat(hash2,tmp);
		}
		printf("\nthe decode_out_token:%s\n",hash2);
		write(sock,encode_out_token,16);

		AES_set_decrypt_key(md,128,&aes);
		AES_ecb_encrypt((unsigned char*)encode_out_token,(unsigned char*)test_str,&aes , AES_DECRYPT);
		printf("decode token: %s\n%s\n",test_str,encode_out_token);

	}
	
	printf("close socket\n");
	close(sock);
	pthread_exit(NULL);
}

char* get_host_ip(char*addr){
	struct ifaddrs* ifAddrStruct = NULL;
	void *tmp=NULL;

	getifaddrs(&ifAddrStruct);

	while(ifAddrStruct !=NULL ){
		if(ifAddrStruct->ifa_addr->sa_family ==AF_INET && !strcmp(ifAddrStruct->ifa_name,"eth0")){
			tmp = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
			inet_ntop(AF_INET,tmp,addr,INET_ADDRSTRLEN);
			return addr;
		}
		ifAddrStruct=ifAddrStruct->ifa_next;
	}

	return NULL;
}
