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
	int epfd = epoll_create(1024);
	if(epfd < 0){
		perror("epoll create error");
		return -1;
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = lfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

	int nready;
	struct epoll_event events[1024];
	int n;
	char buf[1024];
	while(1){
		nready = epoll_wait(epfd, events, 1024, -1);
		if(nready < 0){
			if(errno == EINTR){
				continue;
			}
			break;
		}

		for(int i=0; i< nready; ++i){
			int sockfd = events[i].data.fd;
			if(sockfd == lfd){
				int cfd = accept(lfd, NULL, NULL);

				ev.events = EPOLLIN;
				ev.data.fd = cfd;

				epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
				continue;
			}


			n = read(sockfd, buf, sizeof(buf));
			if(n <= 0){
				close(sockfd);
				epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
				continue;
			}
			memset(buf, 0x00, sizeof(buf));
			printf("n == [%d], buf == %s ", n, buf);
			for(int i=0; i<n; ++i){
				buf[i] = toupper(buf[i]);	
			}
			write(sockfd, buf, n);


		}
	}
	close(epfd);
	/**/
	close(lfd);
	return 0;
}
