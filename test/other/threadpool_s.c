/*************************************************************************
	> File Name: threadpool_s.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Tue 13 Oct 2020 04:32:41 AM PDT
 ************************************************************************/
#include"threadpool_s.h"
threadpool *thrpool = NULL;
int beginnum = 0;

void *threadrun(void *arg){

	threadpool *pool =(threadpool*)arg;
	int taskpos = 0;
	pooltask *task = (pooltask*)malloc(sizeof(pooltask));

	while(1){

		pthread_mutex_lock(&thrpool->pool_lock);

		while(thrpool->job_num <= 0 && !thrpool->shutdown){
			pthread_cond_wait(&thrpool->not_empty_task,&thrpool->pool_lock);
		}

		if(thrpool->job_num){
			taskpos =(thrpool->job_pop++)%thrpool->max_job_num;

			memcpy(task, &thrpool->tasks[taskpos],sizeof(pooltask));

			task->arg = task;
			thrpool->job_num--;
			pthread_cond_signal(&thrpool->empty_task);
		}

		if(thrpool->shutdown){

			pthread_mutex_unlock(&thrpool->pool_lock);
			free(task);
			pthread_exit(NULL);
		}

		pthread_mutex_unlock(&thrpool->pool_lock);

		task->task_func(task->arg);
	}
}

void create_threadpool(int threadsnum, int maxtasknum){

	printf("begin call %s -----\n",__func__);
	thrpool = (threadpool*)malloc(sizeof(threadpool));

	thrpool->max_job_num = maxtasknum;
	thrpool->job_num = 0;
	thrpool->tasks = (pooltask*)malloc(sizeof(pooltask)*maxtasknum);
	thrpool->job_push = 0;
	thrpool->job_pop = 0;

	thrpool->threads_num = threadsnum;
	thrpool->threads = (pthread_t *)malloc(sizeof(pthread_t)*threadsnum);
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	for(int i=0;i<threadsnum;++i){
		pthread_create(&thrpool->threads[i], &attr, threadrun, (void*)thrpool);
	}
	thrpool->shutdown = 0;
	pthread_mutex_init(&thrpool->pool_lock, NULL);
	pthread_cond_init(&thrpool->empty_task, NULL);
	pthread_cond_init(&thrpool->not_empty_task, NULL);
	printf("end call %s -----\n",__func__);
}

void destroy_threadpool(threadpool *pool){

	pool->shutdown = 1;
	pthread_cond_broadcast(&pool->not_empty_task);

	pthread_cond_destroy(&pool->not_empty_task);
	pthread_cond_destroy(&pool->empty_task);
	pthread_mutex_destroy(&pool->pool_lock);

	free(pool->tasks);
	free(pool->threads);
	free(pool);

}

void addtask(threadpool *pool){

	pthread_mutex_lock(&pool->pool_lock);

	//actual job too many should wait
	while(pool->job_num >= pool->max_job_num){
		pthread_cond_wait(&pool->empty_task,&pool->pool_lock);
	}

	int taskpos = (pool->job_push++)%pool->max_job_num;
	pool->tasks[taskpos].tasknum = beginnum++;
	pool->tasks[taskpos].arg = (void*)&pool->tasks[taskpos];
	pool->tasks[taskpos].task_func = taskrun;
	pool->job_num++;

	pthread_mutex_unlock(&pool->pool_lock);

	pthread_cond_signal(&pool->not_empty_task);
}

void taskrun(void *arg){

	pooltask *task = (pooltask*)arg;
	int num = task->tasknum;
	printf("[%ld] task %d is running\n", pthread_self(), num);
	
	//
	sleep(3);
	//
	
	printf("[%ld] task %d is done\n", pthread_self(), num);
}

int main(){

	create_threadpool(3, 20);
	for(int i=0; i<50; ++i){
		addtask(thrpool);
	}

	sleep(20);
	destroy_threadpool(thrpool);
	
}
