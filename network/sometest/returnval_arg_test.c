/*************************************************************************
	> File Name: returnval_arg_test.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Fri 16 Oct 2020 06:52:46 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>

int *func(int *arg){
	
	printf("before %08x\n",arg);
	arg = (int *)malloc(sizeof(arg));
	printf("after %08x\n",arg);
	return arg;
}

int main(int argc, char* argv[]){
	int *ret = func(ret);
	printf("%08x\n",ret);
	return 0;
}
