/*************************************************************************
	> File Name: epoll_service.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Sat 10 Oct 2020 03:16:40 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<ctype.h>
#include <sys/epoll.h>

#define SERVICEPORT 8888
#define MAXCONQUE 128

int main(int argc, char* argv[]){
	int lfd = socket(AF_INET, SOCK_STREAM, 0);

	if(lfd < 0){
		perror("socket error");
		return -1;
	}

	int opt = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

	struct sockaddr_in serv_addr;
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVICEPORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(ret < 0){
		perror("bind error");
		return -1;
	}

	listen(lfd, MAXCONQUE);

	/**/
	while(1){

	}
	/**/
	return 0;
}
