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
#define _EVENT_SIZE_ 1024

int gepfd;

typedef struct my_event{
	int fd;
	int events;
	void (*call_back)(int fd, int events, void *arg);
	void *arg;
	char buf[1024];
	int buflen;
	int epfd;
}myevent;

myevent myevents[_EVENT_SIZE_+1];

void eventadd(int fd, int events, void(*call_back)(int, int, void *), void *arg, myevent *ev){

	ev->fd = fd;
	ev->events = events;
	ev->call_back = call_back;

	struct epoll_event epv;
	epv.events = events;
	epv.data.ptr = ev;
	epoll_ctl(gepfd, EPOLL_CTL_ADD, fd, &epv);
}

void eventset(int fd, int events, void(*call_back)(int, int, void *), void *arg, myevent *ev){
	ev->fd = fd;
	ev->events = events;
	ev->call_back = call_back;

	struct epoll_event epv;
	epvevents = events;
	epv.data.ptr = ev;
	epoll_ctl(gepfd, EPOLL_CTL_MOD, fd, &epv);
}

void eventdel(myevent *ev, int fd, int events){

	ev->fd = 0;
	ev->events = 0;
	ev->call_back = NULL;
	memset(ev->buf, 0x00, sizeof(ev->buf));
	buflen = 0;

	struct epoll_event epv;
    epv.data.ptr = NULL;
    epv.events = events;
    epoll_ctl(gepfd,EPOLL_CTL_DEL,fd,&epv);
}

void initAccept(int fd, int events, void *arg){
	int i;
	structsockaddr_in addr;
	socklen_t len = sizeof(addr);
	int cfd = accept(fd, (struct sockaddr*)&addr, &len);

	for(i=0; i< _EVENT_SIZE_; ++i){
		if(myevents[i].fd == 0){
			break;
		}
	}

	eventadd(cfd, EPOLLIN, readData, &myevents[i], &myevents[i]);
}

void readData(int fd, int events, void *arg){
	myevent *ev =arg;

	ev->buflen = read(fd, ev->buf, sizeof(ev->buf));
	if(ev->buflen > 0){
		eventset(fd, EPOLLOUT, senddata, arg, ev);
	}else if(ev->buflen == 0){
		close(fd);
		eventdel(ev, fd, EPOLLIN);
	}
}

void sendData(int fd, int events, void *arg){
	myevent *ev= arg;
	write(fd, ev->buf, ev->buflen);
	eventset(fd, EPOLLIN, readData, arg, ev);
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
	gepfd = epoll_create(1024);
	if(epfd < 0){
		perror("epoll create error");
		return -1;
	}

	//struct epoll_event ev;
	//ev.events = EPOLLIN;
	//ev.data.fd = lfd;
	//epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

	int nready;
	struct epoll_event events[1024];
	int n;
	char buf[1024];

	evnetadd(lfd, EPOLLIN, initAccept, &myevents[_EVENT_SIZE_], &myevents[_EVENT_SIZE_])
	
	while(1){
		nready = epoll_wait(gepfd, events, 1024, -1);
		if(nready < 0){
			if(errno == EINTR){
				continue;
			}
			break;
		}

		for(int i=0; i< nready; ++i){
			myevent *me = events[i].data.ptr;
			if(me->events & events[i].events){
				me->call_back(me->fd, me->events, me);
			}
		//	int sockfd = events[i].data.fd;
		//	if(sockfd == lfd){
		//		int cfd = accept(lfd, NULL, NULL);

		//		ev.events = EPOLLIN;
		//		ev.data.fd = cfd;

		//		epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
		//		continue;
		//	}


		//	n = read(sockfd, buf, sizeof(buf));
		//	if(n <= 0){
		//		close(sockfd);
		//		epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
		//		continue;
		//	}
		//	memset(buf, 0x00, sizeof(buf));
		//	printf("n == [%d], buf == %s ", n, buf);
		//	for(int i=0; i<n; ++i){
		//		buf[i] = toupper(buf[i]);	
		//	}
		//	write(sockfd, buf, n);


		}
	}
	close(gepfd);
	/**/
	close(lfd);
	return 0;
}
