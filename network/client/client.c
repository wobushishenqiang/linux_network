/*************************************************************************
	> File Name: client.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Wed 30 Sep 2020 02:29:20 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main(int argc, char* argv[]){

	int cfd = socket(AF_INET, SOCK_STREAM, 0);
	if(cfd < 0){
		perror("socket error");
		return -1;
	}

	//int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8888);
	inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);

	int ret = connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(ret < 0){
		perror("connect error");
		return -1;
	}

	int n = 0;
	char buf[256];
	while(1){
		memset(buf, 0x00, sizeof(buf));
		n = read(STDIN_FILENO, buf, sizeof(buf));

		write(cfd, buf, n);

		memset(buf ,0x00,sizeof(buf));
		n = read(cfd, buf, sizeof(buf));
		if(n <= 0){
			printf("read error or server closed, n == [%d]\n", n);
		}
		printf("n == [%d], buf == %s \n",n ,buf);
	}
	close(cfd);
	return 0;
}
