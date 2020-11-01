/*************************************************************************
	> File Name: staticvar_test.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Tue 20 Oct 2020 02:44:14 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>

void func(int arg){
	static int a;
	printf("%d\n",a);
	a += arg;
	printf("%d\n",a);
}

int main(){

	func(1);
	func(1);
	return 0;
}
