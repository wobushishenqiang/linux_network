/*************************************************************************
	> File Name: threadpool.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Wed 14 Oct 2020 04:02:18 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<pthread.h>
#include<signal.h>
#include<errno.h>
#include"threadpool.h"

#define CHECK_TIME 10
#define MIN_WAIT_TASK_NUM 10
#define DEFAULT_THREAD_VARY 10
#define BEGIN_TASKNUM 1
#define true 1
#define false 0

typedef struct{

	void *(*task)(void *);
	void *arg;
	//int tasknum;
} threadpool_task_t;

struct threadpool_t{

	pthread_mutex_t lock;
	pthread_mutex_t thread_counter;

	pthread_cond_t queue_not_full;
	pthread_cond_t queue_not_empty;

	pthread_t *threads;
	pthread_t adjust_tid;
	threadpool_task_t *task_queue;

	int min_thr_num;
	int max_thr_num;
	int live_thr_num;
	int busy_thr_num;
	int wait_exit_thr_num;

	int queue_front;
	int queue_tail;
	int queue_size;
	int queue_max_size;

	int shutdown;
};

int threadpool_free(threadpool_t *pool);

threadpool_t *threadpool_create(int min_thr_num, int max_thr_num, int jobqueue_max_size){

	threadpool_t *pool = NULL;
	do{
		pool = (threadpool_t *)malloc(sizeof(threadpool_t));
		if(pool == NULL){
			printf("malloc threadpool failed\n");
			btreak;
		}


		pool->min_thr_num = min_thr_num;
		pool->max_thr_num = max_thr_num;
		pool->busy_thr_num = 0;
		pool->live_thr_num = min_thr_num;
		pool->wait_exit_thr_num = 0;
		pool->queue_size = 0;
		pool->queue_max_size = jobqueue_max_size;
		pool->queue_front = 0;
		pool->queue_tail = 0;
		pool->shutdown = false;
	}while(0);

	int ret=threadpool_free(pool);
	if(ret != 0){
		printf("free threadpool failed\n");
	}

	return NULL;
}


#if 1


void *my_task(void *arg){
	//int tasknum = (threadpool_task_t *)task_st->tasknum;
	//void *arg = (threadpool_task_t *)task_st 
	printf("thread %ld working ont task[%d]\n", pthread_self(), *(int *)arg);
	sleep(1);
	printf("thread %ld ending.\n", pthread_self());
	return NULL;
}

int main(int argc, char* argv[]){
	threadpool_t thrp = threadpool_create(3,100,100);
	printf("pool inited\n");

	int num[50];
	for(int i=0;i<50;++i){
		num[i] = i;
		printf("add task %d \n",i);
		threadpool_add(thrp, my_task,(void*)&num[i]);
	}

	sleep(20);
	threadpool_destroy(thrp);
	return 0;
}

#endif
