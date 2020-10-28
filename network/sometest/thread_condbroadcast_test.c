/*************************************************************************
	> File Name: threadcond_test.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Tue 13 Oct 2020 06:59:12 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<pthread.h>

pthread_cond_t cond;
pthread_mutex_t mutex;
int i=0;
void *threadwork(void *arg){
	
	printf("[%ld] func in\n",pthread_self());
	pthread_mutex_lock(&mutex);

	printf("[%ld] mutex in\n",pthread_self());
	while(i == 0){
		printf("[%ld] cond wait\n",pthread_self());
		pthread_cond_wait(&cond, &mutex);
		printf("[%ld] cond wait out\n",pthread_self());
		break;
	}

	//sleep(3);
	--i;
	printf("[%ld][%d]\n",pthread_self() ,i);

	pthread_mutex_unlock(&mutex);
}

int main(int argc, char* argv[]){

	printf("-----\n");
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	pthread_t threads[5];
	for(int n=0;n<5;n++){
		pthread_create(&threads[n],NULL,threadwork,NULL);
		pthread_detach(threads[n]);
	}
	

	sleep(5);
	printf("signal send\n");
	++i;
	pthread_cond_broadcast(&cond);
	sleep(5);
	printf("signal send\n");
	++i;
	pthread_cond_broadcast(&cond);

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	return 0;
}
