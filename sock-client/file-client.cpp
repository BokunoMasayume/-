#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<fcntl.h>

#define MAXBUF 1024

int main(){
	int sock = socket(AF_INET , SOCK_STREAM ,IPPROTO_TCP);
        char serv_ip[16]={0};
	int serv_port;
	printf("Please inut server ip port ,such as '127.0.0.1 8777':\n");
	scanf("%s %d",serv_ip,&serv_port);
	struct sockaddr_in serv_addr;
	memset(&serv_addr ,0 ,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(serv_ip);
	serv_addr.sin_port = htons(serv_port);

	connect(sock , (struct sockaddr*)&serv_addr , sizeof(serv_addr));

	char buffer[MAXBUF];
	read(sock,buffer , MAXBUF);
	printf("%s\n",buffer);
	memset(buffer ,0 ,MAXBUF);
	scanf("%s",buffer);

	int fp = open(buffer,O_WRONLY|O_CREAT);

	write(sock , buffer ,MAXBUF);
    int bcount=0;	
	while((bcount = read(sock,buffer,MAXBUF))>0){
		write(fp,buffer,bcount);
		//memset(buffer,0,MAXBUF);
	}
	printf("file tranfer successed");
	close(fp);
	close(sock);

	return 0;
}

