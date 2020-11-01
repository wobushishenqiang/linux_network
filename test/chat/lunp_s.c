//
// Created by xyx on 10/17/20.
//

#include "lunp_s.h"

void perr_exit(const char *err_str)
{
    perror(err_str);
    exit(-1);
}

int Socket(int family, int type, int protocol)
{
    int ret;

    if( (ret = socket(family, type, protocol)) < 0)
        perr_exit("socket error");

    return ret;
}

int SocketTCP4()
{
    int ret;

    if( (ret = socket(AF_INET, SOCK_STREAM , 0)) < 0)
        perr_exit("socket error");

    return ret;
}

void Bind(int fd, const struct sockaddr *sa,socklen_t salen)
{
    if(bind(fd, sa, salen) < 0)
        perr_exit("bind error");
}

void Listen(int fd, int backlog)
{
    char *ptr;

    if( (ptr = getenv("LISTENQ")) != NULL)
        backlog = atoi(ptr);

    if(listen(fd, backlog) < 0)
        perr_exit("listen error");
}

int Accept(int fd,struct sockaddr *sa_get, socklen_t *salen_get)
{
    int afd;
    /*
    do{
        if( (afd = accept(fd, sa_get, salen_get)) < 0)
        {
            if (errno == EPROTO || error == ECONNABORTED)
                continue;
            perr_exit("accept error");
        }
        break;
    }while(1);
    */
    again:
    if( (afd =accept(fd, sa_get, salen_get)) < 0)
    {
        if(errno == EPROTO || errno == ECONNABORTED || errno == EINTR)
            goto again;
        else
            perr_exit("accept error");
    }
    return afd;
}

void Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
    if(connect(fd, sa, salen) < 0)
        perr_exit("connect error");
}

ssize_t Recvform(int fd, void *buf, size_t buflen, int flags, struct sockaddr *sa_get, socklen_t *salen_get)
{
    ssize_t len;
    if( (len = recvfrom(fd, buf, buflen, flags, sa_get, salen_get)))
        perr_exit("recvfrom error");
    return len;
}
void Sendto(int fd, const void *buf, size_t buflen, int flags, const struct sockaddr *sa, socklen_t salen)
{
    if(sendto(fd, buf, buflen, flags, sa, salen) != (ssize_t)buflen)
        perr_exit("sendto error");
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,struct timeval *timeout)
{
    int n;

    if( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
        perr_exit("select error");

    return n;
}
int SelcetOnlyRead(int nfds, fd_set *readfds, struct timeval *timeout)
{
    int n;

    if( (n = select(nfds, readfds, NULL, NULL,timeout)) < 0)
        perr_exit("select error");

    return n;
}

int Poll(struct pollfd *fdarray, unsigned long nfds, int timeout)
{
    int n;

    if( (n = poll(fdarray, nfds, timeout)) < 0)
        perr_exit("poll error");

    return n;
}

int tcp4Listen()
{
    int lfd = SocketTCP4();

    struct sockaddr_in serv;
    bzero(&serv, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(SERV_PORT);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    Bind(lfd, (struct sockaddr *)&serv, sizeof(serv));

    Listen(lfd, LISTENQ);

    return lfd;
}

ssize_t Read(int fd,void *buf, size_t buflen)
{
    ssize_t n;
    if( (n = read(fd, buf , buflen)) == -1)
        perror("read error");


    return n;
}

static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

static ssize_t my_read(int fd, char *ptr)
{
    if(read_cnt <= 0)
    {
        again:

        if( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0){
            if(errno == EINTR)
                goto again;
            return -1;
        }else if (read_cnt == 0)
            return 0;
        read_ptr = read_buf;

    }
    read_cnt--;
    *ptr = *read_ptr++;
    return 1;
}

static ssize_t readline(int fd, void *buf, size_t buflen)
{
    ssize_t n,rc;
    char c, *ptr;

    ptr = buf;

    for(n = 1; n < buflen; ++n)
    {
        if( (rc = my_read(fd, &c)) == 1){
            *ptr++ = c;
            if(c == '\n')
                break;
        }else if(rc == 0){
            *ptr = 0;
            return n-1;
        }else
            return -1;
    }

    *ptr = 0;
    return n;
}

ssize_t ReadLine(int fd, void *buf, size_t buflen)
{
    ssize_t n;

    if( (n = readline(fd, buf, buflen)) < 0)
        perror("readline error");

    return n;
}

static ssize_t readn(int fd, void *buf, size_t buflen)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr = buf;
    nleft = buflen;
    while(nleft > 0)
    {
        if( (nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR)
                nread = 0;
            else
                return -1;
        }else if(nread == 0)
            break;

        nleft -= nread;
        ptr += nread;
    }

    return buflen-nleft;
}

ssize_t Readn(int fd, void *buf, size_t buflen)
{
    ssize_t n;

    if( (n = readn(fd, buf, buflen)) < 0)
        perror("readn error");

    return n;
}

void Write(int fd, void *buf, size_t buflen)
{
    if(write(fd, buf, buflen) != buflen)
        perror("write error");
}

static ssize_t writen(int fd,const void *buf, size_t buflen)
{
    size_t nleft;
    ssize_t nwrite;
    const char *ptr;

    ptr = buf;
    nleft = buflen;
    while(nleft > 0)
    {
        if( (nwrite = write(fd, ptr, nleft)) <= 0){
            if(nwrite < 0 && errno == EINTR)
                nwrite = 0;
            else
                return -1;
        }

        nleft -= nwrite;
        ptr += nwrite;
    }

    return buflen-nleft;
}

void Writen(int fd, void *buf, size_t buflen)
{
    if(writen(fd, buf, buflen) != buflen)
        perror("writen error");
}
