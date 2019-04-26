#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<inttypes.h>
#include<string.h>
#include<termios.h>
//#include<pthread.h>

#define X 40
#define Y 40
#define BLOCK 1
#define UNBLOCK 0

unsigned char map[X][Y]={{0}};
unsigned char buffmap[X][Y];

char input='a';

struct termios init_term(int type);
void clean_term(struct termios settings);
void init_map();
void update_map();
void show_map();
//void show_comfor_map();
void set_init_map();
int main(int argc,char**argv){ 
	struct termios settings;
	init_map();
	char al='a';
	while(al!='q'){
		printf("type \'p\' start with default map\ntype \'s\' set origin map\ntype \'q\' quit program");
		al = getchar();
		if(al=='p'){
			settings = init_term(UNBLOCK);
			while(al!='q'){
				show_map();
				update_map();
				usleep(400000);
				al = getchar();
			}
			clean_term(settings);
		}
		else if(al=='s'){
			settings = init_term(BLOCK);
			set_init_map();
			clean_term(settings);
		}
		
	}
//	set_init_map();
	
/*	while(1){
		show_map();
		update_map();
		usleep(1000000);

	}
*/	
	
	return 0;

}	

void init_map(){

	map[0][2]=1;
	map[1][2]=1;
	map[2][2]=1;
	map[1][0]=1;
	map[2][1]=1;
	return ;
}
void update_map(){
	int count = 0;
	for(int i=0;i<X;i++){
		for(int j=0;j<Y;j++){
			count = 0;
			for(int dx=-1;dx<2;dx++){
				for(int dy=-1;dy<2;dy++){
					if(dx==0 && dy==0)continue;
					if(i+dx<0 || j+dy<0 || i+dx >=X || j+dy >= Y)continue;
//printf("x:%d y:%d  ",i+dx,j+dy);
					if(map[i+dx][j+dy]!=0){
						count++;
					}
				}//dy

			}//dx
//printf("count:%d\n",count);
			if(count==3)buffmap[i][j]=1;
			else if(count==2 && map[i][j]==1)buffmap[i][j]=1;
			else buffmap[i][j]=0;
		}//j
	}//i
	memcpy(map,buffmap,X*Y);
}


void show_map(){
	printf("\033[1;1H");
	for(int i=0;i<X;i++){
		for(int j=0;j<Y;j++){
			if(map[i][j]==0)printf(" ");
			else if(map[i][j]==1)printf("\u2587");
//			else printf("|");
//			else printf("#");			
		}
		printf("\n");
	}
//	printf("\033[0;0H");
}

void set_init_map(){
	int x=0,y=0;
//	show_comfor_map();
	show_map();
	printf("\033[1;1H");
	while(input!='q'){
		input = getchar();
		if(input==' '){
			map[x][y]=map[x][y]==1?0:1;
		}
		else if(input=='w'){
			x--;
			x = x<0?0:x;
		}else if(input=='s'){
			x++;
			x = x>=X?X-1:x;

		}else if(input=='a'){
			y--;
			y = y<0?0:y;
		}else if(input=='d'){
			y++;
			y = y>=Y?Y-1:y;
		}
		show_map();
//		map[x][y]=2;
		printf("\033[%d;%dH",x+1,y+1);
		usleep(1000);
		
	}

}

struct termios init_term(int type){
	
	struct termios my_settings,old_settings;
	tcgetattr(STDIN_FILENO,&my_settings);
	old_settings =  my_settings;
	my_settings.c_lflag &= ~ICANON;
	my_settings.c_lflag &= ~ECHO;
//	my_settings.c_lflag &=~ISIG;
	if(type==UNBLOCK){
		my_settings.c_cc[VMIN]=0;
		my_settings.c_cc[VTIME]=0;
	}
	if(type==BLOCK){
		my_settings.c_cc[VMIN]=1;
		my_settings.c_cc[VTIME]=0;
	}
	tcsetattr(STDIN_FILENO,TCSANOW,&my_settings);
	return old_settings;
}

void clean_term(struct termios settings){
	tcsetattr(STDIN_FILENO,TCSANOW,&settings);
}
