/*************************************************************************
	> File Name: udp_service.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Wed 14 Oct 2020 04:57:55 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<ctype.h>
#include<arpa/inet.h>
#include<netinet/in.h>

#define SERVICE_PORT 8888
#define MAX_WAIT_QUEUE_SIZE 128
#define BUFLEN 1024

int main(int argc, char* argv[]){

	int cfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(cfd < 0){

		perror("socket error");
		return -1;
	}

	struct sockaddr_in serv_addr;
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family =  AF_INET;
	serv_addr.sin_port = htons(SERVICE_PORT);
	inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);


	int n;
	socklen_t len = sizeof(serv_addr);
	char buf[BUFLEN];
	while(1){

		memset(buf, 0x00, sizeof(buf));
		n = read(STDIN_FILENO, buf, sizeof(buf));
		sendto(cfd, buf, n, 0, (struct sockaddr *)&serv_addr, len);

		memset(buf, 0x00, sizeof(buf));

		n = recvfrom(cfd, buf, sizeof(buf), 0, NULL, NULL);
		printf("n == [%d], buf == %s \n", n, buf);

	}

	close(cfd);
	return 0;
}
