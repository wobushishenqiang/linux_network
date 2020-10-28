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
#include<sys/epoll.h>
#include<fcntl.h>
#include<time.h>

#define SERVICEPORT 8888
#define MAXCONQUE 128
#define MAX_EVENT_SIZE 1024
#define BUFLEN 1024
#define CHECK_OT_NUM 100
#define MAX_NONACTIVE_TIME 60

void recvdata(int fd, int events, void *arg);
void senddata(int fd, int events, void *arg);
int g_epfd;
int g_lfd;

typedef struct my_event{
	int fd;
	int events;
	void *arg;
	void (*call_back)(int fd, int events, void *arg);
	
	int status;
	long last_active;

	char buf[BUFLEN];
	int buflen;

	char* cip;
	uint16_t cport;

}myevent;

myevent myevents[MAX_EVENT_SIZE+1];


void eventinit(myevent *ev, int fd, void *arg){

	ev->fd = fd;
	ev->events = 0;
	ev->call_back = NULL;
	ev->status = 0;
	ev->arg = arg;
	memset(ev->buf, 0, sizeof(ev->buf));
	ev->buflen = 0;
	ev->cip = NULL;
	ev->cport = 0;

	ev->last_active = time(NULL);
}

void eventset(int efd, int events, void(*call_back)(int, int, void *), myevent *ev){

	ev-> call_back = call_back;
	ev->last_active = time(NULL);

	struct epoll_event epv = {0, {0}};
	epv.events = ev->events = events;
	epv.data.ptr = ev;
	
	int op;
	if(ev->status == 1){
		op = EPOLL_CTL_MOD;
	}else{
		op = EPOLL_CTL_ADD;
		ev->status = 1;
	}

	if(epoll_ctl(efd, op, ev->fd, &epv) < 0){
		printf("event add faild");
	}else{
		//pritnf("event add success");
	}
	
}

void eventdel(int efd,myevent *ev){

	struct epoll_event epv = {0, {0}};

	if(ev->status != 1){
		return;
	}
	epv.data.ptr = ev;
	ev->status = 0;
	
    epoll_ctl(efd, EPOLL_CTL_DEL, ev->fd, &epv);
}

void acceptconn(int lfd, int events, void *arg){
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	int cfd , index;
	cfd = accept(lfd, (struct sockaddr *)&client_addr, &len);

	for(index=0; index<MAX_EVENT_SIZE; ++index){
		if(myevents[index].status == 0){
			break;
		}
	}

	if(index == MAX_EVENT_SIZE){
		printf("%s: max connect limit [%d]\n", __func__, MAX_EVENT_SIZE);
		return;
	}

	int flags = 0;
	flags = fcntl(cfd, F_GETFL, 0);
	flags |= O_NONBLOCK;

	if((flags = fcntl(cfd, F_SETFL, flags)) < 0){
		printf("%s: fcntl nonblocking failed, %s\n", __func__, strerror(errno));
		return;
	}

	eventinit(&myevents[index], cfd, &myevents[index]);
	
	eventset(g_epfd, EPOLLIN, recvdata, &myevents[index]);
	
	myevents[index].cip = inet_ntoa(client_addr.sin_addr);
	myevents[index].cport = ntohs(client_addr.sin_port);

	printf("new connect [%s:%d][time:%ld], pos[%d]\n",
			inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port),
			myevents[index].last_active,
			index);
	
}

void recvdata(int fd, int events, void *arg){
	int len;
	myevent *ev = (myevent *)arg;

	memset(ev->buf, 0x00, sizeof(ev->buf));
	len = read(fd, ev->buf, sizeof(ev->buf));

	if(len > 0){
		ev->buflen = len;
		eventset(g_epfd, EPOLLOUT, senddata, ev);
		printf("[%s:%d][time:%ld]-->recv:%s\n", ev->cip, ev->cport, ev->last_active, ev->buf);
	}else if(len == 0){
		close(ev->fd);
		eventdel(g_epfd, ev);
		printf("[%s:%d] closed.\n", ev->cip, ev->cport);
	}else{
		close(ev->fd);
		eventdel(g_epfd, ev);
		printf("[%s:%d] read error[%d]:%s\n", ev->cip, ev->cport, errno, strerror(errno));
	}
}

void senddata(int fd, int events, void *arg){
	int len;
	myevent *ev = (myevent *) arg;

	for(int i=0; i<ev->buflen; ++i){
		ev->buf[i] = toupper(ev->buf[i]);
	}

	len = write(fd, ev->buf, ev->buflen);

	if(len > 0){
		eventset(g_epfd, EPOLLIN, recvdata, ev);
		printf("[%s:%d][time:%ld]<--send:%s\n", ev->cip, ev->cport, ev->last_active, ev->buf);
	}else{
		close(ev->fd);
		eventdel(g_epfd, ev);
		printf("[%s:%d] send error[%d]:%s\n", ev->cip, ev->cport, errno, strerror(errno));
	}
}

void initlistensocket(){

	g_lfd = socket(AF_INET, SOCK_STREAM, 0);

	if(g_lfd < 0){
		perror("socket error");
		exit(-1);
	}


	int opt = 1;
	setsockopt(g_lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	
	eventinit(&myevents[MAX_EVENT_SIZE], g_lfd, &myevents[MAX_EVENT_SIZE]);
	eventset(g_epfd, EPOLLIN,acceptconn, &myevents[MAX_EVENT_SIZE]);

	struct sockaddr_in serv_addr;
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVICEPORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(g_lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(ret < 0){
		perror("bind error");
		exit(-1);
	}

	listen(g_lfd, MAXCONQUE);

}

int main(int argc, char* argv[]){


	g_epfd = epoll_create(MAX_EVENT_SIZE+1);
	if(g_epfd < 0){
		perror("epoll create error");
		return -1;
	}

	initlistensocket();

	
	struct epoll_event events[1024];

	int checkpos = 0;
	int i;
	while(1){
		long now = time(NULL);
		for(i=0; i<CHECK_OT_NUM; ++i, ++checkpos){
			if(checkpos == MAX_EVENT_SIZE){
				checkpos = 0;
			}

			if(myevents[checkpos].status != 1){
				continue;
			}

			long duration = now - myevents[checkpos].last_active;

			if(duration >= MAX_NONACTIVE_TIME){
				close(myevents[checkpos].fd);
				printf("[%s:%d] timeout\n", myevents[checkpos].cip, myevents[checkpos].cport);
				eventdel(g_epfd, &myevents[checkpos]);
			}
		}

		int nready = epoll_wait(g_epfd, events, 1024, -1);
		if(nready < 0){
			if(errno == EINTR){
				continue;
			}
			printf("epoll_wait error, exit\n");
			break;
		}

		for(int i=0; i< nready; ++i){
			myevent *ev = events[i].data.ptr;

			//读就绪事件
            if ((events[i].events & EPOLLIN) && (ev->events & EPOLLIN)) 
			{
                //ev->call_back(ev->fd, events[i].events, ev->arg);
                ev->call_back(ev->fd, events[i].events, ev);
            }
			//写就绪事件
            if ((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT))
			{
                //ev->call_back(ev->fd, events[i].events, ev->arg);
                ev->call_back(ev->fd, events[i].events, ev);
            }  
		}
	}

	close(g_epfd);
	close(g_lfd);
	return 0;
}
