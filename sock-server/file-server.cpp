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
	int serv_sock = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);

	struct sockaddr_in serv_addr;
	memset(&serv_addr ,0 ,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_addr.sin_port = htons(8777);

	bind(serv_sock , (struct sockaddr*)&serv_addr , sizeof(serv_addr));
    //func bind passed the int 'size'
	listen(serv_sock,20);

	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size = sizeof(clnt_addr);

	int clnt_sock = accept(serv_sock , (struct sockaddr*)&clnt_addr , &clnt_addr_size);
	//func accept passed szie's point

	char str[] = "Please input file's name: ";
	write(clnt_sock ,str  ,sizeof(str));
   
    char buffer[MAXBUF];

	read(clnt_sock , buffer , MAXBUF);
    
	int fp = open(buffer,O_RDONLY);
	if(fp<0){
		perror("ERROR-FAIL-TO-OPEN-FILE:");
		exit(-1);
	}
	memset(buffer,0,MAXBUF);
	int bcount=0;
	while((bcount=read(fp ,buffer,MAXBUF))>0){
		//memset(buffer , 0 ,MAXBUF);
		write(clnt_sock , buffer ,bcount);
	}

	shutdown(clnt_sock,1);
	read(clnt_sock,buffer,MAXBUF);
	
	close(fp);
	close(clnt_sock);
	close(serv_sock);

	return 0;
}
