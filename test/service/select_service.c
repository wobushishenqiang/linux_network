/*************************************************************************
	> File Name: select_service.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Sat 03 Oct 2020 07:32:16 AM PDT
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
#define SERVICEPORT 8888
#define MAXCONNUM 128

int main(int argc,char* argv[]){
 
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if(lfd < 0){
		perror("socket error");
		return -1;
	}

	int opt = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVICEPORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int ret = bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(ret < 0){
		perror("bind error");
		return -1;
	}

	listen(lfd,  MAXCONNUM);

	fd_set readfds;
	fd_set tmpfds;

	FD_ZERO(&readfds);
	FD_ZERO(&tmpfds);

	FD_SET(lfd, &readfds);

	int maxfd = lfd;
	int nready;
	int cfd;
	int sockfd;
	int n;
	char buf[1024];
	while(1){
	
		tmpfds = readfds;
		nready = select(maxfd+1, &tmpfds, NULL, NULL, NULL );
		if(nready < 0){

			if(errno == EINTR){
				continue;
			}

			break;
		}

		if(FD_ISSET(lfd, &tmpfds)){
			cfd = accept(lfd, NULL, NULL);
			FD_SET(cfd, &readfds);

			if(maxfd < cfd){
				maxfd = cfd;
			}
			if(--nready == 0){
				continue;
			}
		}

		for(int i=lfd+1; i<=maxfd; ++i){
			sockfd = i;
			if(FD_ISSET(sockfd,&tmpfds)){
				memset(buf, 0x00, sizeof(buf));
				if(n <= 0){
					close(sockfd);

					FD_CLR(sockfd, &readfds);
				}else{
					printf("n == [%d], buf == %s ", n, buf);
					for(int j=0; j<n; ++j){
						buf[j] = toupper(buf[j]);
					}

					write(sockfd, buf, n);
				}
				
				if(--nready == 0){
					break;
				}
			}
		}

	}
	close(lfd);
	return 0;
}
