/*************************************************************************
	> File Name: threadpool.h
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Wed 14 Oct 2020 03:52:44 AM PDT
 ************************************************************************/

#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

typedef struct threadpool_t threadpool_t;

threadpool_t *threadpool_create(int min_thr_num, int max_thr_num, int jobqueue_max_size);


int threadpool_add(threadpool_t *pool, void*(*task)(void *arg), void *arg);

int threadpool_destroy(threadpool_t *pool);

int threadpool_all_threadnum(threadpool_t *pool);

int threadpool_busy_threadnum(threadpool_t *pool);
#endif
