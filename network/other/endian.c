/*************************************************************************
	> File Name: endian.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Wed 30 Sep 2020 01:37:04 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>

union{
	short s;
	char c[sizeof(short)];
}un;

int main(int argc, char* argv[]){

	un.s = 0x0102;
	if(sizeof(short) == 2){
		printf("%d,%d,%d\n",un.c[0],un.c[1],un.s);
		if(un.c[0] == 1 && un.c[1] == 2){
			printf("big-endian\n");
		}else if (un.c[0] == 2 && un.c[1] == 1){
			printf("small-endian\n");
		}else{
			printf("unknown\n");
		}

	}else
		printf("size of short == [%ld]\n",sizeof(short));
	return 0;
}
