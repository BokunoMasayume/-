#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mysql/mysql.h>
#include<openssl/md5.h>

int main(){

	char name[50];
	unsigned char passwd[16];
	char hash[33];
	char tmp[3];
//	char in_name[50];
	char in_passwd[50];
	char re_passwd[50];
	printf("Please input user name:\n");
	scanf("%49s",name);
	printf("Ur user name is %s\n",name);
	fflush(stdin);
	printf("\nPlease input user password:\n");
	scanf("%49s",in_passwd);
	fflush(stdin);
	printf("\nPlease input user password again:\n");
	scanf("%49s",re_passwd);
	if((strcmp(in_passwd,re_passwd))){
		printf("U input different password!\n");
		return 0;
	}
	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx,name,strlen(name));
	MD5_Update(&ctx,in_passwd , strlen(in_passwd));
	MD5_Final(passwd , &ctx);
	snprintf(hash,sizeof(hash),"%02x",passwd[0]);
	for(int i=1;i<16;i++){
	//	printf("%02x ",passwd[i]);
		snprintf(tmp,sizeof(tmp) , "%02x",passwd[i]);
		strcat(hash,tmp);
	}
	printf("\nur passwd hash is %s\n",hash);
	
	char cmd[200];
	snprintf(cmd , sizeof(cmd),"insert into user(name , passwd) values('%s','%s')",name , hash);
	printf("\nthe cmd Is %s\n",cmd);
	MYSQL mysql;
	mysql_init(&mysql);
	mysql_real_connect(&mysql , "localhost" , "root","mushroom","lab4",0,NULL,0);
	if(mysql_query(&mysql , cmd)){
		printf("Fail to insert user ");
		printf("ERROR:%s",mysql_error(&mysql));
	}

	return 0;
}

