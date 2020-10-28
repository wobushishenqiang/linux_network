/*************************************************************************
	> File Name: service.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Wed 30 Sep 2020 02:14:09 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<ctype.h>
int main(int argc, char* argv[]){

	//int socket(int domain, int type, int protocol);
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if(lfd < 0){
		perror("socket error");
		return -1;
	}

	//int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
	struct sockaddr_in serv_addr;
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8888);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int ret = bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(ret < 0){
		perror("bind error");
		return -1;
	}

	//int listen(int sockfd, int backlog);
	listen(lfd, 128);

	//int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	int cfd = accept(lfd, (struct sockaddr *)&client_addr, &len);
	char cIP[16];
	memset(cIP, 0x00, sizeof(cIP));
	printf("client IP: [%s], port: [%d]\n", inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, cIP, sizeof(cIP)), ntohs(client_addr.sin_port));
	printf("lfd = [%d], cfd ==[%d]\n", lfd, cfd);

	int n = 0;
	char buf[1024];
	while(1){
		memset(buf, 0x00, sizeof(buf));
		n = read(cfd, buf, sizeof(buf));
		if(n <= 0){
			printf("read error or connect close, n == [%d]\n", n);
			break;
		}

		printf("n == [%d], buf == %s \n", n, buf);

		for(int i = 0; i < n; i++){
			buf[i] = toupper(buf[i]);
		}

		write(cfd, buf, n);
	}
	close(lfd);
	close(cfd);

	return 0;
}
