/*************************************************************************
	> File Name: threadpool_s.h
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Tue 13 Oct 2020 04:24:42 AM PDT
 ************************************************************************/
#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<pthread.h>

typedef struct _pooltask{

	int tasknum;
	void *arg;
	void (*task_func)(void* arg);
}pooltask;

typedef struct _threadpool{

	int max_job_num;
	int job_num;
	pooltask *tasks;
	int job_push;
	int job_pop;

	int threads_num;
	pthread_t *threads;
	int shutdown;
	pthread_mutex_t pool_lock;
	pthread_cond_t empty_task;
	pthread_cond_t not_empty_task;
}threadpool;

void create_threadpool(int threadsnum, int maxtasknum);
void destroy_threadpool(threadpool *pool);
void addtask(threadpool *pool);
void taskrun(void *arg);

#endif
