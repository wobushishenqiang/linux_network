/*************************************************************************
  > File Name: fork_service.c
  > Author: xyx
  > Mail:719660015@qq.com 
  > Created Time: Thu 01 Oct 2020 02:14:21 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<ctype.h>
#include<signal.h>
#include<sys/wait.h>
#define SERVICEPORT 8888
#define MAXCONNUM 128

void child_sighandle(int signo){
	while(1){
		pid_t wpid = waitpid(-1, NULL, WNOHANG);

		if(wpid > 0){
			printf("child is quit, wpid == [%d]\n", wpid);
		}else if(wpid == 0){
			break;
		}else if(wpid == -1){
			break;
		}
	}

}

int main(int argc, char* argv){

	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if(lfd < 0){
		perror("socket error");
		return -1;
	}

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

	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	sigprocmask(SIG_BLOCK, &mask, NULL);

	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	char cIP[16];
	int cfd;
	while(1){

		cfd = accept(lfd,(struct sockaddr *)&client_addr, &len);
		memset(cIP, 0x00, sizeof(cIP));
		printf("client IP == [%s], PORT == [%d]\n",inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, cIP, sizeof(cIP)), ntohs(client_addr.sin_port));
		if(cfd < 0)
			continue;
		else{
			pid_t pid = fork();
			if(pid < 0){
				perror("fork error");
				return -1;
			}else if(pid > 0){
				close(cfd);

				struct sigaction act;
				act.sa_handler = child_sighandle;
				sigemptyset(&act.sa_mask);
				act.sa_flags = 0;
				sigaction(SIGCHLD, &act, NULL);
				sigprocmask(SIG_UNBLOCK, &mask, NULL);

			}else if(pid == 0){

				close(lfd);
				int n;
				char buf[1024];
				while(1){
					memset(buf, 0x00, sizeof(buf));
					n = read(cfd, buf, sizeof(buf));
					if(n <= 0){
						printf("read error or close, n == [%d]\n",n);
						break;
					}

					printf("%s:%d : n == [%d], buf == %s ",cIP ,ntohs(client_addr.sin_port), n, buf);
					for(int i = 0; i < n; ++i){
						buf[i] = toupper(buf[i]);
					}

					write(cfd, buf, n);
				}
				close(cfd);
				exit(0);
			}
		}
	}	

	close(lfd);
	return 0;
}
