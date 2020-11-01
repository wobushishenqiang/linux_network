/*************************************************************************
	> File Name: thead_test.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Sat 03 Oct 2020 05:36:24 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<pthread.h>

void *thread_work(void *arg){
	sleep(5);
	int i = *(int *)arg;
	printf("i == [%d]\n", i);
	pthread_exit(NULL);
}

int main(int argc, char* argv[]){

	pthread_t threadID;
	int i = 0;
	while(1){
		sleep(1);
		pthread_create(&threadID, NULL, thread_work, &i);
		pthread_detach(threadID);
		++i;
	}
	return 0;
}
