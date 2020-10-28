/*************************************************************************
  > File Name: lv_service.c
  > Author: xyx
  > Mail:719660015@qq.com 
  > Created Time: Thu 15 Oct 2020 05:31:39 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<ctype.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<event2/event.h>

const int g_serv_port = 8888;
const int g_serv_max_wait_size = 128;
struct event_base *base = NULL;

void recvData(evutil_socket_t fd, short events, void *arg);
void writeData(evutil_socket_t fd, short events, void *arg);

void connectionAccept(evutil_socket_t fd, short events, void *arg){
	struct sockaddr_in client;
	socklen_t len = sizeof(client);

	int cfd = accept(fd, (struct sockaddr *)&client, &len);
	printf("[%s]:[%d] connect.\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

	if(cfd > 0){

		struct event *recv_event = event_new(base, cfd, EV_READ|EV_PERSIST, recvData, recv_event);
		if(NULL == recv_event){
			printf("connect event new faild\n");
			return;
		}
		event_add(recv_event, NULL);
	}
}

void recvData(evutil_socket_t fd, short events, void *arg){

	int n;
	char buf[1024];
	memset(buf, 0x00, sizeof(buf));
	n = read(fd, buf, sizeof(buf));

	if(n <= 0){
		struct event *ev = (struct event *)arg;
		close(fd);
		event_del(ev);
	}else{
		write(fd, buf, n);
	}
}
int main(int argc, char* argv[]){

	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if(lfd < 0){
		perror("socket error");
		return -1;
	}

	int opt = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	struct sockaddr_in serv;
	bzero(&serv, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(g_serv_port);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(lfd, (struct sockaddr*)&serv, sizeof(serv));
	if(ret < 0){
		perror("bind error");
		return -1;
	}

	listen(lfd, g_serv_max_wait_size);

	base = event_base_new();
	if(NULL == base){
		printf("event base new faild\n");
		return -1;
	}

	struct event *conn_event = event_new(base, lfd, EV_READ|EV_PERSIST, connectionAccept, NULL);

	event_add(conn_event, NULL);

	event_base_dispatch(base);

	event_base_free(base);
	event_free(conn_event);
	return 0;
}
