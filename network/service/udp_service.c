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

	int sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sfd < 0){

		perror("socket error");
		return -1;
	}

	struct sockaddr_in serv_addr;
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family =  AF_INET;
	serv_addr.sin_port = htons(SERVICE_PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	struct sockaddr_in client_addr;
	int n;
	socklen_t len;
	char buf[BUFLEN];
	while(1){

		memset(buf, 0x00, sizeof(buf));
		len = sizeof(client_addr);

		n = recvfrom(sfd, buf, sizeof(buf), 0, (struct sockaddr*)&client_addr, &len);
		
		printf("n == [%d], buf == %s\n", n, buf);

		for(int i=0;i<n;++i){

			buf[i] = toupper(buf[i]);
		}

		sendto(sfd, buf, n, 0, (struct sockaddr *)&client_addr, len);
	}

	close(sfd);
	return 0;
}
