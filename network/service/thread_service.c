/*************************************************************************
	> File Name: thread_service.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Sat 03 Oct 2020 01:03:30 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<signal.h>
#include<sys/wait.h>
#include<pthread.h>
#define SERVICEPORT 8888
#define MAXCONNUM 128

struct clientinfo{
	int cfd;
	struct sockaddr_in client_addr;
};

struct clientinfo cli_info[MAXCONNUM];

void findindex(int *index){
	while(1){
		if(cli_info[*index].cfd == -1){
			break;
		}
		*index = *index+1 < MAXCONNUM ? *index+1 : 0; 
	}
}


void *thread_work(void *arg){
	int index = *(int *)arg;
	int cfd = cli_info[index].cfd;
	int n;
	char buf[1024];
	char cIP[16];
	while(1){
		memset(cIP, 0x00, sizeof(cIP));
		memset(buf, 0x00, sizeof(buf));
		n = read(cfd, buf, sizeof(buf));
		if(n <= 0){
			printf("read error or close, n == [%d\n]", n);
			break;
		}

		printf("%s:%d : n == [%d], buf == %s",\
				inet_ntop(AF_INET, &cli_info[index].client_addr.sin_addr.s_addr, cIP, sizeof(cIP)),\
				ntohs(cli_info[index].client_addr.sin_port),\
				n, buf);

		for(int i = 0; i < n; ++i){
			buf[i] = toupper(buf[i]);
		}
		write(cfd, buf, n);
	}
	close(cfd);
	cli_info[index].cfd = -1;
	pthread_exit(NULL);
}

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
	serv_addr.sin_family =  AF_INET;
	serv_addr.sin_port = htons(SERVICEPORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(lfd, (struct sockaddr * )&serv_addr, sizeof(serv_addr));
	if(ret < 0){
		perror("bind error");
		return -1;
	}

	listen(lfd, MAXCONNUM);
	
	pthread_t threadID;
	int cfd;
	int index = 0;
	for(int i = 0; i < MAXCONNUM; ++i){
		cli_info[i].cfd = -1;
	}

	while(1){
		findindex(&index);
		cli_info[index].cfd = accept(lfd, (struct sockaddr *)&cli_info[index].client_addr, sizeof((struct sockaddr *)&cli_info[index].client_addr));
		pthread_create(&threadID, NULL, thread_work, &index);
		pthread_detach(threadID);
	}


	close(lfd);
	return 0;
}
