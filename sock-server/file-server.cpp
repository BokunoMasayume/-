#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<pthread.h>

#define MAXBUF 1024

void *tranfer_file_2clnt(void* clnt_sock);

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

	while(1){

		int clnt_sock = accept(serv_sock , (struct sockaddr*)&clnt_addr , &clnt_addr_size);
		//func accept passed szie's point

		pthread_t thrd;
		pthread_create(&thrd , NULL,tranfer_file_2clnt ,(void*)&clnt_sock);
   
    		
	}
	close(serv_sock);

	return 0;
}

void *tranfer_file_2clnt(void *clnt_sock){
	int sock = *(int *)clnt_sock;
	char str[] = "Please input file's name: ";
	write(sock ,str , sizeof(str));

	char buffer[MAXBUF]={0};
	read(sock ,buffer ,MAXBUF);

	int fp = open(buffer,O_RDONLY);
	if(fp<0){
		printf("ERROR-FAIL-TO-OPEN-FILE:%s\n",buffer);
		pthread_exit(NULL);
	}

	int bcount = 0;
	while((bcount=read(fp,buffer,MAXBUF))>0){
		write(sock , buffer,bcount);
	}

	shutdown(sock,1);
	read(sock,buffer,MAXBUF);

	close(fp);
	close(sock);
	pthread_exit(NULL);
}
