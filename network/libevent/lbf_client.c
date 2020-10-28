/*************************************************************************
	> File Name: lbf_client.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Sat 17 Oct 2020 12:50:56 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include<event2/event.h>
#include<event2/buffer.h>
#include<event2/bufferevent.h>

static void cmd_msg_cb(int fd, short events, void *arg);
static void server_msg_cb(struct bufferevent *bev, void *arg);
static void event_cb(struct bufferevent *bev, short events, void*arg);

int main(int argc, char *argv[])
{
	if(argc < 3){

		printf("please input 2 parameter\n");
		return -1;
	}

	struct event_base *base = event_base_new();

	struct bufferevent *bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

	struct event *ev_cmd = event_new(base, STDIN_FILENO, EV_READ|EV_PERSIST, cmd_msg_cb, (void*)bev);
	
	event_add(ev_cmd, NULL);

	struct sockaddr_in serv;
	memset(&serv, 0x00, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(atoi(argv[2]));
	inet_aton(argv[1], &serv.sin_addr);

	bufferevent_socket_connect(bev, (struct sockaddr *)&serv, sizeof(serv));
	bufferevent_setcb(bev, server_msg_cb, NULL, event_cb, (void*)ev_cmd);
	bufferevent_enable(bev, EV_READ|EV_PERSIST);

	event_base_dispatch(base);
	event_free(ev_cmd);
	bufferevent_free(bev);
	event_base_free(base);
	printf("finished \n");
	return 0;

}

static void cmd_msg_cb(int fd, short events, void *arg)
{
	 char msg[1024];

	 int ret = read(fd, msg, sizeof(msg));
	 if(ret < 0)
	 {
		perror("read error");
		return ;
	 }

	 struct bufferevent *bev = (struct bufferevent *)arg;
	 bufferevent_write(bev, msg, ret);
}

static void server_msg_cb(struct bufferevent *bev, void *arg)
{
	char msg[1024];
	memset(msg, 0x00, sizeof(msg));
	size_t len = bufferevent_read(bev, msg, sizeof(msg));
	printf("n == [%ld], buf == %s", len, msg);
}


static void event_cb(struct bufferevent *bev, short events, void *user_data)
{
	if (events & BEV_EVENT_EOF) {
		printf("Connection closed.\n");
	} else if (events & BEV_EVENT_ERROR) {
		printf("Got an error on the connection: %s\n",
		    strerror(errno));/*XXX win32*/
	}
	/* None of the other events can happen here, since we haven't enabled
	 * timeouts */
	bufferevent_free(bev);
}

