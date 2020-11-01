/*************************************************************************
	> File Name: lv_test.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Fri 16 Oct 2020 03:47:07 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<event2/event.h>

int main(int argc, char *argv[]){

	int i = 0;
	const char ** pmethods = event_get_supported_methods();
	while(pmethods[i]!=NULL){

		printf("%s \t", pmethods[i]);
		i++;
	}
	printf("\n");


	struct event_base *base = event_base_new();
	if(base == NULL){

		printf("event_base_new failed\n");
		return -1;
	}

	const char * pbase_method = event_base_get_method(base);
	printf("%s \n", pbase_method);

	event_base_free(base);
	return 0;
}
