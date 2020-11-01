/*************************************************************************
	> File Name: poll_service.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Wed 07 Oct 2020 01:19:29 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<errno.h>
#include<poll.h>
#define SERVICEPORT 8888
#define MAXCONNUM 128

int main(int argc,char* argv[]){

	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if(lfd < 0){
		perror("socket error");
		return -1;
	}

	int opt = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR,&opt, sizeof(int));

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

	listen(lfd, MAXCONNUM);

	struct pollfd client[1024];
	client[0].fd = lfd;
	client[0].events = POLLIN;

	for(int i=1; i<1024; ++i){
		client[i].fd = -1;
	}

	int nready;
	int cfd;
	int maxi = 0;
	int n;
	char buf[1024];
	while(1){
		nready = poll(client, maxi+1,-1);
		if(nready < 0){
			if(errno == EINTR){
				continue;
			}
			break;
		}

		if(client[0].revents == POLLIN){
			cfd = accept(lfd, NULL, NULL);
			int i;
			for(i=0; i<1024; ++i){
				if(client[i].fd == -1){
					client[i].fd = cfd;
					client[i].events = POLLIN;
					break;
				}
			}

			if(i == 1024){
				close(cfd);
				continue;
			}
			
			if(maxi < i){
				maxi = i;
			}

			if(--nready == 0){
				continue;
			}
		}

		for(int i=1; i<=maxi; ++i){
			int sockfd = client[i].fd;
			if(client[i].fd == -1){
				continue;
			}

			if(client[i].revents == POLLIN){
				n = read(sockfd, buf, sizeof(buf));
				if(n <= 0){
					close(sockfd);
					client[i].fd = -1;
					if(i == maxi){
						maxi = i-1;
					}
					continue;
				}else{
					memset(buf, 0x00, sizeof(buf));
					printf("n == [%d], buf == %s ", n, buf);
					for(int j=0; j<n; ++j){
						buf[j] = toupper(buf[j]);
					}
					write(sockfd, buf, n);
				}

				

			}
		}
	}
	return 0;
}
