//
// Created by xyx on 10/17/20.
//

#ifndef LUNP_S_H
#define LUNP_S_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ctype.h>
#include <string.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <sys/poll.h>
#include <sys/select.h>
#include <sys/epoll.h>

#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define SERV_PORT 8888
#define SERV_PORT_STR "8888"

#define LISTENQ 1024
#define BUFFSIZE 8192
#define MAXLINE 4096

int Socket(int family, int type, int protocol);
int SocketTCP4();
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void Listen(int fd, int backlog);
int Accept(int fd, struct sockaddr *sa_get, socklen_t *salen_get);

void Connect(int fd, const struct sockaddr *sa,socklen_t salen);

ssize_t Recvform(int fd, void *buf, size_t buflen, int flags, struct sockaddr *sa_get, socklen_t *salen_get);
void Sendto(int fd, const void *buf, size_t buflen, int flags, const struct sockaddr *sa, socklen_t salen);

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,struct timeval *timeout);
int SelcetRead(int ns, fd_set *readfds, struct timeval *timeout);
int Poll(struct pollfd *fdadday, unsigned long nfds, int timeout);

int tcp4Listen();

ssize_t Read(int fd,void *buf, size_t buflen);
ssize_t ReadLine(int fd, void *buf, size_t buflen);
ssize_t Readn(int fd, void *buf, size_t buflen);

void Write(int fd, void *buf, size_t buflen);
void Writen(int fd, void *buf, size_t buflen);

#endif //LUNP_S_H
